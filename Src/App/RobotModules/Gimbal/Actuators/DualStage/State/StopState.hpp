#pragma once

#include "DualStage.hpp"
#include "FSMState.hpp"
#include "StmLog.hpp"
#include <cstdint>

namespace GIMBAL {

class StopState : public FSMState<FSMState_e> {
public:
    StopState(DualStage *_gimbal) : FSMState<FSMState_e>(FSMState_e::STOP), gimbal_(_gimbal) {}

    void enter() final
    {
        gimbal_->stop();
        LOG::info("GimbalStop", "enter");
    }

    void run() final { gimbal_->stop(); }

    void exit() final { LOG::info("GimbalStop", "exit"); }

    FSMState_e checkChange() final { return FSMState_e::STOP; }

private:
    DualStage *gimbal_;
};

} // namespace GIMBAL
