#ifndef PATIENT_H
#define PATIENT_H
#include <QString>
#include <QObject>
class Patient
{

public:
    Patient();
    ~Patient();

    QString GetSex();
    QString GetName();
    QString GetBirth();
    QString GetPost();
    QString GetAddress();
    QString GetDate1();
    QString GetDate2();
    QString GetDate3();
    QString GetPhoto();
    QString GetGuominshi();
    QString GetHistory();
    QString GetPhone();

    void SetSex(QString sex);
    void SetName(QString name);
    void SetBirth(QString birth);
    void SetPost(QString post);
    void SetAddress(QString address);
    void SetDate1(QString date1);
    void SetDate2(QString date2);
    void SetDate3(QString date3);
    void SetPhoto(QString photo);
    void SetGuomishi(QString guominshi);
    void SetHistory(QString history);
    void SetPhone(QString Phone);

private:
    QString sex;
    QString name;
    QString birth;
    QString address;
    QString photo;
    QString post;        //邮编
    QString phone;       //电话
    QString guominshi;   //过敏史
    QString date1;        //出诊日期
    QString date2;        //复诊日期
    QString date3;        //入院日期
    QString history;      //病史
};

#endif // PATIENT_H
