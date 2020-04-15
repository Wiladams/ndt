
#include "blend2d.h"

int main(int argc, char* argv[]) {
  BLImage img(480, 480, BL_FORMAT_PRGB32);

  // Attach a rendering context into `img`.
  BLContextCreateInfo createInfo;
  createInfo.reset();
  createInfo.threadCount = 4;

  BLContext ctx(img, createInfo);

  // Clear the image.
  ctx.setCompOp(BL_COMP_OP_SRC_COPY);
  ctx.fillAll();

  // Fill some path.
  BLPath path;
  path.moveTo(26, 31);
  path.cubicTo(642, 132, 587, -136, 25, 464);
  path.cubicTo(882, 404, 144, 267, 27, 31);

  ctx.setCompOp(BL_COMP_OP_SRC_OVER);
  ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
  ctx.fillPath(path);

  // Detach the rendering context from `img`.
  //ctx.flush(BL_CONTEXT_FLUSH_SYNC);
  // BUGBUG - ctx.end() silently fails 
  ctx.end();

  // Let's use some built-in codecs provided by Blend2D.
  BLImageCodec codec;
  codec.findByName("BMP");
  img.writeToFile("bl-getting-started-1.bmp", codec);

  return 0;
}
