



　ほぼ猫と一緒なので、相違だけ


REG8 joymng_getstat(void);

　ボタン1 = ビット6
　ボタン2 = ビット5

　ボタン3 = ビット7
　ボタン4 = ビット4

　ボタン3,4は実際には接続されていないが、ソーサリアンとかビットチェックしてる
ソフトで使う事ができる (ただそれだけ)





REG8 mousemng_getstat(SINT16 *x, SINT16 *y, BRESULT clear);

　左 = ビット0
　右 = ビット1

　押下で ビットが立つ (NP2と逆)





BRESULT scrnmng_setcolormode(BRESULT fullcolor);

　8bpp対応の場合は、fullcolor = TRUEで 16bit以上のスクリーンモードに切り替え
　8bpp非対応の場合は 何もせずに SUCCESSを返せば良い


