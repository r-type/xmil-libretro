
enum {
	IEVENT_SIO			= 0,
	IEVENT_DMA,
	IEVENT_CTC0,
	IEVENT_CTC1,
	IEVENT_CTC2,
	IEVENT_SUBCPU,

	IEVENT_MAX
};

typedef struct {
	UINT	dummy;
} IEVENT;


#ifdef __cplusplus
extern "C" {
#endif

extern	IEVENT	ievent;

void ievent_reset(void);
void ievent_progress(void);
void ievent_setbit(UINT bit);

#define ievent_set(id)			ievent_setbit(1 << (id))

#ifdef __cplusplus
}
#endif

