class Point {
  public:
    int x;
    int y;
    Path p;

    Point() {
      reset();

      BallVector *up3 = (BallVector *)malloc(sizeof(BallVector));
      up3->a = AXIS_Y;
      up3->scalar = -1;
      up3->next = NULL;

      BallVector *up2 = (BallVector *)malloc(sizeof(BallVector));
      up2->a = AXIS_Y;
      up2->scalar = -1;
      up2->next = up3;

      BallVector *left = (BallVector *)malloc(sizeof(BallVector));
      left->a = AXIS_X;
      left->scalar = -1;
      left->next = up2;

      BallVector *up = (BallVector *)malloc(sizeof(BallVector));
      up->a = AXIS_Y;
      up->scalar = -1;
      up->next = left;

      p.vectors = up;
    }

    bool collideWalls(BallVector *v, int next_x, int next_y) {
      bool collision = false;

      // vector moves on X axis, check for collision
      if (v->a == AXIS_X) {
        if (next_x < 0) {
          collision = true;
          p.collide_y();
          x = v->scalar;
        } else if (next_x > MAX_X) {
          collision = true;
          p.collide_y();
          x = MAX_X + v->scalar;
        } else {
          x = next_x;
        }
      } else { // vector moves on Y axis, check for collision
        if (next_y < 0) {

          collision = true;
          p.collide_x();
          y = v->scalar;
        } else if (next_y > MAX_Y) {
          reset();
          delay(500);
          /*collision = true;
          p.collide_x();
          y = MAX_Y + v->scalar;*/
        } else {
          y = next_y;
        }
      }

      return collision;
    }

    bool collideBumper(BallVector *v, int next_x, int next_y) {

      bool collision = false;

      if (next_y == bumper_y) {
        if (next_x >= bumper_x && next_x <= bumper_x + (bumper_mul * 8)) {
          collision = true;
          int bumper_index = 7 * ((float)(next_x - bumper_x) / (float)(bumper_mul * 8));
          copyVectors(bumperVectors[bumper_index]);
        }
      }

      return collision;
    }

    bool collideBlocks(BallVector *v, int next_x, int next_y) {
      bool collision = false;

      for (int i = 0; i < BLOCK_COUNT; i++)
      {
        if (blocks[i].active) {
          if (next_x >= blocks[i].x && next_x <= (blocks[i].x + blocks[i].w)
              && next_y >= blocks[i].y && next_y <= blocks[i].y + blocks[i].h) {
            collision = true;
            blocks[i].active = false;
            blocks[i].fade = 1;
            switch (v->a) {
              case AXIS_X:
                p.collide_y();
                break;
              case AXIS_Y:
                p.collide_x();
                break;
            }
          }
        }
      }

      return collision;
    }


    void dump_coords() {
      display.setCursor(0, 0);

      char str[8]; // probably too big
      sprintf(str, "%u %u", x, y);
      display.print(str);
    }

    void tick() {
      BallVector *v = p.vectors;

      while (v) {
        int next_x = x;
        int next_y = y;

        switch (v->a) {
          case AXIS_X:
            next_x += v->scalar;
            break;
          case AXIS_Y:
            next_y += v->scalar;
            break;
        }

        collideWalls(v, next_x, next_y) ||
        collideBumper(v, next_x, next_y) ||
        collideBlocks(v, next_x, next_y);

        v = v->next;
      };
    }

    void copyVectors(BallVector * new_v) {
      // first free the original vectors
      BallVector *v = p.vectors;
      BallVector *next_v = NULL;
      while (v) {
        next_v = v->next;

        free(v);
        v = next_v;
      }

      // copy the new vectors
      BallVector *prev_v = NULL;
      while (new_v) {
        v = newVector();
        v->a = new_v->a;
        v->scalar = new_v->scalar;
        v->next = prev_v;
        prev_v = v;
        new_v = new_v->next;
      }

      p.vectors = v;
    }

    void reset() {
      x = 64;
      y = 32;
    }
};
