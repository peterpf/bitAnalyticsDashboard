enum ApplicationState {
  STARTUP = 0,
  RUNNING = 1,
  PAUSING = 2,
  SHUTDOWN = 3
};

String getStateName(ApplicationState state) {
  switch(state) {
    case RUNNING: return "RUNNING";
    case STARTUP: return "STARTUP";
    case PAUSING: return "PAUSING";
    case SHUTDOWN: return "SHUTDOWN";
    default: return "UNKNOWN";
  }
}
