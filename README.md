RX 18 -> yellow -> green
TX 19 -> blue -> white

### Cool fonts I like:
- u8g2_font_bubble_tr (really cool for splash screens)
- u8g2_font_helvR10_tr (very readable, should really try this one)
- u8g2_font_6x12_tf (monospace)

### Weird font rendering stuff going on
When the cursor is set to a position, the text will actually render
from the left bottom corner, so 0,0 will actually render text that
can't be viewed.
Solution, for now:
```cpp
U8G2 u8g2;
u8g2.setFont(u8g2_font_9x18_tf);
int ascent = u8g2.getAscent();
// will probably vary from font to font tho, right now I'm using
// monospace and that's enough
```
