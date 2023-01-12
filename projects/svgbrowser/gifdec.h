#pragma once

#ifndef GIFDEC_H
#define GIFDEC_H

#include "blend2d.h"
#include "bstream.h"

#include <stdint.h>
#include <sys/types.h>



struct gd_Palette {
    int size{};
    uint8_t colors[0x100 * 3]{ 0 };
};

struct gd_GCE {
    uint16_t delay{};
    uint8_t tindex{};
    uint8_t disposal{};
    int input{};
    int transparency{};
};

struct gd_GIF {
    ndt::BStream fd{};
    
    size_t anim_start{0};
    uint16_t width{ 0 };
    uint16_t height{ 0 };
    uint16_t depth{ 0 };
    uint16_t loop_count{ 0 };
    uint8_t bgindex{ 0 };
    gd_GCE gce{};
    gd_Palette* palette{ nullptr };
    gd_Palette lct{};
    gd_Palette gct{};
    void (*plain_text)(
        struct gd_GIF* gif, uint16_t tx, uint16_t ty,
        uint16_t tw, uint16_t th, uint8_t cw, uint8_t ch,
        uint8_t fg, uint8_t bg
        ) {nullptr};
    void (*comment)(struct gd_GIF* gif) {nullptr};
    void (*application)(struct gd_GIF* gif, ndt::DataChunk id, ndt::DataChunk auth) {nullptr};
    uint16_t fx{ 0 };
    uint16_t fy{ 0 };
    uint16_t fw{ 0 };
    uint16_t fh{ 0 };

    uint8_t* canvas{ nullptr };
    uint8_t* frame{ nullptr };

    gd_GIF(const ndt::DataChunk& inChunk);
    ~gd_GIF();

    void rewind();
    bool is_bgcolor(void *acolor);
    void render_frame(uint8_t* buffer);
    int getFrame();
    bool readImage();
    
private:
    void render_frame_rect(uint8_t* buffer);
    void disposeFrame();
    int read_image_data(int interlace);
    uint16_t get_key(int key_size, uint8_t& sub_len, uint8_t& shift, uint8_t& abyte);
    
    void discard_sub_blocks();
    void read_ext();
    void read_application_ext();
    void read_comment_ext();
    void read_graphic_control_ext();
    void read_plain_text_ext();
};


//#ifdef __cplusplus
//extern "C" 
bool parseGIF(void* bytes, const size_t sz, BLImage &outImage);
//#endif

#endif // GIFDEC_H
