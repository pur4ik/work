#include "../bp/bp.h"
#include "../mm/myvisa.h"
#include "../buev/buev_udp.h"
#include "../upvs/upvs.h"
#include "../load/socket.h"
#include "psi_results.h"
#include <atomic>
#include "../upvs/upvs_data.h"
#include "loader.h"

class Psi {
public:

struct psi_info_flags
{
    bool _220_1{false};
    bool _220_start{false};
    bool _220_continue{false};

    bool _221_1{false};
    bool _221_start{false};
    bool _221_continue{false};

    bool _224 = false;

    bool _225 = false;

    bool base{true};
};
psi_info_flags flags;

int init (); //функция инициализации и проверки работоспособности всех систем
int _220(psi_results &results);
int _221 (psi_results &results);
int _224 (psi_results &results, upvs_data &data);
int _225 (psi_results &results, upvs_data &data);


};
extern Psi psi;