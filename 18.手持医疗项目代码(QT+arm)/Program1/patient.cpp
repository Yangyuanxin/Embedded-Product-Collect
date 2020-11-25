#include "patient.h"

Patient::Patient()
{
}

QString Patient::GetSex()
{
    return sex;
}
QString Patient::GetName()
{
    return name;
}
QString Patient::GetAddress()
{
    return address;
}
QString Patient::GetDate1()
{
    return date1;
}
QString Patient::GetDate2()
{
    return date2;
}
QString Patient::GetDate3()
{
    return date3;
}
QString Patient::GetPhoto()
{
    return photo;
}
QString Patient::GetHistory()
{
    return history;
}
QString Patient::GetGuominshi()
{
    return guominshi;
}
QString Patient::GetPost()
{
    return post;
}
QString Patient::GetBirth()
{
    return birth;
}
QString Patient::GetPhone()
{
    return phone;
}


void Patient::SetSex(QString sex)
{
    this->sex = sex;
}
void Patient::SetName(QString name)
{
    this->name = name;
}
void Patient::SetAddress(QString address)
{
    this->address = address;
}
void Patient::SetDate1(QString date1)
{
    this->date1 = date1;
}
void Patient::SetDate2(QString date2)
{
    this->date2 = date2;
}
void Patient::SetDate3(QString date3)
{
    this->date3 = date3;
}

void Patient::SetPhoto(QString photo)
{
    this->photo = photo;
}
void Patient::SetHistory(QString history)
{
    this->history = history;
}

void Patient::SetGuomishi(QString guominshi)
{
    this->guominshi = guominshi;
}
void Patient::SetPost(QString post)
{
    this->post = post;
}
void Patient::SetBirth(QString birth)
{
    this->birth = birth;
}
void Patient::SetPhone(QString phone)
{
    this->phone = phone;
}
