#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace ArmPositions {

//Events
  enum Position {
    PosHome,
    PosBell
  };

  //Point definition.
  struct Points {
    Position name;
    float coordinates[6];
  };

  static Points PointTable[] {
    Position::PosHome,    {-0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310},
    Position::PosBell,    {-0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310},
  }

}

#endif
