#include "upvs.h"

Upvs upvs_request;

void Upvs::recieve_data (upvs_data &data) {
    commander.command ();
    test.parse_response(test.read_file(commander.response_file), data);   
}