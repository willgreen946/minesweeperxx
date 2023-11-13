#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
 * Color pair numbers
 */
enum {
  WIN_BOX = 1,
  BAK_BOX = 2
};

/*
 * Ascii keys
 */
enum {
  ESC = 27,
  RET = 10,
  SPC = 32
};

enum {
  WIN_Y = 24,
  WIN_X = 80,
  GRD_Y = 16,
  GRD_X = 30
};

enum {
  MINES = 99,
  FLAGS = MINES
};

#endif /* __CONFIG_H__ */
