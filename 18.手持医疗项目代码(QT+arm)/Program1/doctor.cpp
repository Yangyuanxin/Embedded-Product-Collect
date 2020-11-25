#include "doctor.h"

Doctor::Doctor()
{

}

QString Doctor::GetTime()
{
    return time;
}
QString Doctor::GetName()
{
    return name;
}
QString Doctor::GetDepartment()
{
	return department;
}
QString Doctor::GetJob()
{
    return job;
}
QString Doctor::GetPhoto()
{
	return photo;
}
QString Doctor::GetExperience()
{
	return experience;
}

void Doctor::SetTime(QString time)
{
    this->time = time;
}
void Doctor::SetName(QString name)
{
	this->name = name;
}
void Doctor::SetDepartment(QString department)
{
	this->department = department;
}
void Doctor::SetJob(QString job)
{
    this->job = job;
}
void Doctor::SetPhoto(QString photo)
{
	this->photo = photo;
}
void Doctor::SetExperience(QString experience)
{
	this->experience = experience;
}
