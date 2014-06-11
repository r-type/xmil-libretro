
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

REG8 joymng_getstat(void);

#ifdef __cplusplus
}
#endif

void joymng_initialize(void);
void joymng_deinitialize(void);
void joymng_axismotion(const SDL_Event *event);
void joymng_buttondown(const SDL_Event *event);
void joymng_buttonup(const SDL_Event *event);
