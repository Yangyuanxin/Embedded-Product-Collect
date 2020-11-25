#include <QCoreApplication>
#include <QDateTime>
#include <QSerialPort>
#include <QTcpSocket>
#include "casicAgnssAidIni.c"

#define SERVER "www.gnss-aide.com"
#define PORT 2621

#define TTY_NAME "COM9"
#define BAUD_RATE 9600

#define AID_INI_LENGTH 66
#define MAX_AGNSS_DATA 4096
#define EXPIRED_SECONDS 3600

#define USERNAME "freetrial"
#define PASSWORD "123456"

static char requestString[128];
static int downloadTimestamp = 0;
static char agnssData[MAX_AGNSS_DATA];
static int agnssDataSize = 0;
static char aidIniMsg[AID_INI_LENGTH];

static double roughLat = 30.5;
static double roughLon = 120.5;
static double roughAlt = 0;

static int receiverPowerup = 1;

int get_current_timestamp() {
    return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() / 1000;
}

int agnss_required() {
    return receiverPowerup;
}

int agnss_data_expired() {
    int currentTimestamp = get_current_timestamp();
    if (abs(currentTimestamp - downloadTimestamp) >= EXPIRED_SECONDS) {
        printf("AGNSS data is expired.\n");
        return 1;
    }
    else {
        printf("AGNSS data is still valid.\n");
        return 0;
    }
}

int str_to_int(char *str, int size) {
    int n = 0;
    for (int i = 0; i < size; i++) {
        char c = str[i];
        if (c >= '0' && c <= '9') {
            n = 10 * n + c - '0';
        }
    }
    return n;
}

int agnss_data_count_length(char *data, int size) {
    int count = 0;
    int flag = 0;
    for (int i = 0; i < size; i++) {
        if ((unsigned char)data[i] == 0xBA) {
            flag = 1;
        }
        if (flag) {
            count += 1;
        }
    }
    return count;
}

#define AGNSS_DATA_FIRST_LINE "AGNSS data from CASIC."
#define AGNSS_DATA_SECOND_LINE "DataLength: "
int agnss_data_check_length(char *data, int size) {
    printf("Run AGNSS data length check.\n");
    int lineCounter = 0;
    char line[64];
    memset(line, 0, sizeof(line));
    char *p = line;
    int length = 0;
    for (int i = 0; i < size; i++) {
        *p = data[i];
        if (*p == '\n') {
            printf("Current line: %s", line);
            if (lineCounter == 0) {
                if (memcmp(line, AGNSS_DATA_FIRST_LINE, strlen(AGNSS_DATA_FIRST_LINE))) {
                    printf("Error: wrong AGNSS data first line.\n");
                    return 0;
                }
                printf("This is the first line!\n");
            }
            else if (lineCounter == 1) {
                if (memcmp(line, AGNSS_DATA_SECOND_LINE, strlen(AGNSS_DATA_SECOND_LINE))) {
                    printf("Error: wrong AGNSS data second line.\n");
                    return 0;
                }
                printf("This is the second line!\n");

                length = str_to_int(line, p - line);
                printf("Raw agnss data length in data is: %d.\n", length);
                break;
            }
            memset(line, 0, sizeof(line));
            p = line;
            lineCounter += 1;
        }
        else {
            p += 1;
        }
        if (p - line >= 64) {
            printf("Error: line length out of range.\n");
            return 0;
        }
    }

    int countLength = agnss_data_count_length(data, size);
    printf("Raw agnss data length by count is: %d.\n", countLength);
    if (length != 0 && countLength == length) {
        printf("AGNSS data length check PASSED.\n");
        return 1;
    }
    printf("AGNSS data length check FAILED.\n");
    return 0;
}

int agnss_data_download() {
    printf("Download AGNSS data from server.\n");
    QTcpSocket socket;
    socket.setReadBufferSize(1024);
    printf("Try connect to server...\n");
    socket.connectToHost(SERVER, PORT);
    if (socket.waitForConnected()) {
        printf("Connect to the host: %s:%d\n", SERVER, PORT);

        sprintf(requestString, "user=%s;pwd=%s;cmd=eph;", USERNAME, PASSWORD);
        socket.write(requestString, strlen(requestString));
        socket.waitForBytesWritten();
        printf("Send request message: %s\n", requestString);

        char *p = agnssData;
        while (socket.waitForReadyRead()) {
            int size = qMin((int)socket.bytesAvailable(), (int)(MAX_AGNSS_DATA - (p - agnssData)));
            int n = socket.read(p, size);
            printf("Receive %d bytes from server.\n", n);
            if (n > 0) {
                p += n;
                if ((p - agnssData) >= MAX_AGNSS_DATA) {
                    break;
                }
            }
        }
        agnssDataSize = p - agnssData;
        if (agnssDataSize > 0) {
            printf("Receive %d bytes agnss data TOTAL.\n", agnssDataSize);
            printf("*********************\n");
            printf("%s", agnssData);
            printf("\n");
            printf("*********************\n");

            if (agnss_data_check_length(agnssData, agnssDataSize)) {
                downloadTimestamp = get_current_timestamp();
                return 1;
            }
        }
        else {
            printf("Error: no data download from agnss server.\n");
        }
    }
    else {
        printf("Error: can not connect to agnss server.\n");
    }
    return 0;
}

void get_current_datetime(DATETIME_STR *datetime) {
    printf("Get current date time form system.\n");
    QDateTime dt = QDateTime::currentDateTimeUtc();
    // system utc time -> DATETIME_STR
    datetime->valid = 1;
    datetime->year = dt.date().year();
    datetime->month = dt.date().month();
    datetime->day = dt.date().day();
    datetime->hour = dt.time().hour();
    datetime->minute = dt.time().minute();
    datetime->second = dt.time().second();
    datetime->ms = 0;
}

void get_rough_lla(POS_LLA_STR *lla) {
    printf("Get rough LLA from cell id or bluetooth or wifi...\n");
    lla->valid = 1;
    lla->lon = roughLon;
    lla->lat = roughLat;
    lla->alt = roughAlt;
}

void create_aid_ini_message(char *aidIniMsg) {
    printf("Create AID INI message.\n");
    // create time and location message

    DATETIME_STR datetime;
    get_current_datetime(&datetime);

    // create position struct
    POS_LLA_STR lla;
    get_rough_lla(&lla);

    casicAgnssAidIni(datetime, lla, aidIniMsg);
}

void agnss_data_send() {
    printf("Send agnss data to device.\n");
    QSerialPort serial;
    serial.setPortName(TTY_NAME);
    serial.setBaudRate(BAUD_RATE);
    if (serial.open(QSerialPort::ReadWrite) ) {
        printf("Open serial port: %s\n", TTY_NAME);
        // create aid ini message
        create_aid_ini_message(aidIniMsg);
        // send aid ini message
        serial.write(aidIniMsg, AID_INI_LENGTH);
        serial.waitForBytesWritten(1000);
        printf("Send AID INI message.\n");

        // send eph message
        serial.write(agnssData, agnssDataSize);
        serial.waitForBytesWritten(3000);
        printf("Send GPS EPH messages.\n");
        serial.close();
    }
    else {
        printf("Error: can not open serial port: %s.\n", TTY_NAME);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    printf("Hello AGNSS!\n");
    while (1) {
        if (agnss_data_expired()) {
            agnss_data_download();
        }

        if (agnss_required()) {
            agnss_data_send();
        }
        printf("Press any key to continue.\n");
        getchar();
    }
    return app.exec();
}
