//
// Created by Lenovo on 4/9/2026.
//

#ifndef BACKEND_SJF_H
#define BACKEND_SJF_H
#include "scheduler.h"
class SJF : public scheduler {
public:
    void run() override;


};
#endif //BACKEND_SJF_H