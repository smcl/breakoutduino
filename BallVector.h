// axis we're currently travelling in
typedef enum {
  AXIS_X,
  AXIS_Y
} axis;

// one step of a vector which represents a ball's
// direction of travel
typedef struct BallVector {
  axis a;
  int scalar;
  struct BallVector *next;
} BallVector;
