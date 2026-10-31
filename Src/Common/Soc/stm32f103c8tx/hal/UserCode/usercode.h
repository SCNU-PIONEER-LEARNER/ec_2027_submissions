#ifndef USERCODE_H
#define USERCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 基础任务2：验证 UserCode 已被 CMake 链接进构建 */
void UserCode_Init(void);
uint32_t UserCode_GetMagic(void);

#ifdef __cplusplus
}
#endif

#endif /* USERCODE_H */
