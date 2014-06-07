
enum {
	IEVENT_SIO			= 0,
	IEVENT_DMA,
	IEVENT_CTC0,
	IEVENT_CTC1,
	IEVENT_CTC2,
	IEVENT_SUBCPU,

	IEVENT_MAX
};


#ifdef __cplusplus
extern "C" {
#endif

#define	ievent_reset()
void ievent_progress(void);
void ievent_setbit(UINT bit);
void ievent_eoi(void);

#define ievent_set(id)			ievent_setbit(1 << (id))

#ifdef __cplusplus
}
#endif

