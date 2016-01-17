#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 9
#define BLOCK_COUNT (MAX_X/BLOCK_WIDTH)

class Block {
  public:
    int x;
    int y;
    int w;
    int h;
    bool active;
    int fade;
};
