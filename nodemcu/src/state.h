enum ApplicationState {
  STARTUP = 0,
  RUNNING = 1,
  PAUSING = 2,
  CONNECTING = 3
};

String getStateName(ApplicationState state) {
  switch(state) {
    case CONNECTING: return "CONNECTING";
    case RUNNING: return "RUNNING";
    case STARTUP: return "STARTUP";
    case PAUSING: return "PAUSING";
    default: return "UNKNOWN";
  }
}
