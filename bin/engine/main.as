void init()
{
}

void update()
{
}

void draw()
{
    ui.print(10, 10, "FPS: " + engine.getFPS());
    ui.print("fonts/Hack-Regular.ttf:14", 20, 10, "FPS: " + engine.getFPS());
}
