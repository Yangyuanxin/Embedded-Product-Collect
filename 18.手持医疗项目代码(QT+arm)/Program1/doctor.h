#ifndef DOCTOR_H
#define DOCTOR_H
#include <QString>
#include <QObject>
class Doctor
{
public:
    Doctor();
    ~Doctor();

    QString GetTime();
    QString GetName();
    QString GetDepartment();
    QString GetPhoto();
    QString GetExperience();
    QString GetJob();

    void SetTime(QString time);
    void SetName(QString name);
    void SetDepartment(QString department);
    void SetPhoto(QString photo);
    void SetExperience(QString experience);
    void SetJob(QString job);
private:
    QString time;
    QString name;
    QString job;
    QString department;
    QString photo;
    QString experience;
};

#endif // DOCTOR_H
