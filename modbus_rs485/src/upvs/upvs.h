#ifndef UPVS_H
#define UPVS_H

#include <iostream>
#include "netcat.h"
#include "../buev/data.h"

class Upvs {
    public:
    bool upvs_connection = false;

    
    void recieve_data (upvs_data &data);
    void stop ();
};
extern Upvs upvs_request;
#endif