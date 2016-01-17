class Path {
  public:
    BallVector *vectors;

    void collide_x() {
      collide(AXIS_Y);
    }

    void collide_y() {
      collide(AXIS_X);
    }

    void collide(axis collision_axis) {
      BallVector *v = vectors;

      while (v) {

        if (v->a == collision_axis) {
          v->scalar *= -1;
        }

        v = v->next;
      };
    }
};
