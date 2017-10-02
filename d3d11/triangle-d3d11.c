//------------------------------------------------------------------------------
//  triangle-d3d11.c
//------------------------------------------------------------------------------
#include "d3d11entry.h"
#define SOKOL_IMPL
#define SOKOL_D3D11
#define SOKOL_D3D11_SHADER_COMPILER
#define SOKOL_LOG(s) OutputDebugStringA(s)
#include "sokol_gfx.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    /* setup d3d11 app wrapper */
    d3d11_init(640, 480, 1, L"Sokol Triangle D3D11");

    /* setup sokol gfx */
    sg_setup(&(sg_desc){
        .d3d11_device = d3d11_device(),
        .d3d11_device_context = d3d11_device_context(),
        .d3d11_render_target_view_cb = d3d11_render_target_view,
        .d3d11_depth_stencil_view_cb = d3d11_depth_stencil_view
    });

    /* default pass action (clear to grey) */
    sg_pass_action pass_action = { 0 };

    /* a vertex buffer with the triangle vertices */
    const float vertices[] = {
        /* positions            colors */
         0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
            .content = vertices
    });

    /* a shader to render the triangle */
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .vs.source =
            "struct vs_in {\n"
            "  float4 pos: POS;\n"
            "  float4 color: COLOR;\n"
            "};\n"
            "struct vs_out {\n"
            "  float4 color: COLOR0;\n"
            "  float4 pos: SV_Position;\n"
            "};\n"
            "vs_out main(vs_in inp) {\n"
            "  vs_out outp;\n"
            "  outp.pos = inp.pos;\n"
            "  outp.color = inp.color;\n"
            "  return outp;\n"
            "}\n",
        .fs.source = 
            "float4 main(float4 color: COLOR0): SV_Target0 {\n"
            "  return color;\n"
            "}\n"
    });

    /* a pipeline object */
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .vertex_layouts[0] = {
            .stride = 28,
            .attrs = {
                [0] = { .name = "POS", .offset = 0, .format = SG_VERTEXFORMAT_FLOAT3 },
                [1] = { .name = "COLOR", .offset = 12, .format = SG_VERTEXFORMAT_FLOAT4 }
            }
        },
        .shader = shd
    });

    /* a draw state struct with all the resource bindings */
    sg_draw_state draw_state = {
        .pipeline = pip,
        .vertex_buffers[0] = vbuf
    };

    /* the draw loop */
    while (d3d11_process_events()) {
        sg_begin_default_pass(&pass_action, d3d11_width(), d3d11_height());
        sg_apply_draw_state(&draw_state);
        sg_draw(0, 3, 1);
        sg_end_pass();
        sg_commit();
        d3d11_present();
    }
    /* shutdown everything */
    sg_shutdown();
    d3d11_shutdown();
    return 0;
}
