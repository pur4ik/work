#ifndef UPVS_H
#define UPVS_H

#include <iostream>
#include "netcat.h"
#include "data.h"

class Upvs {
    public:

    
    void recieve_data (upvs_data &data);
};
extern Upvs upvs_request;
#endif