enum RequestType {
  Instances = 0,
  CPU_EU = 1,
  CPU_US = 2,
  Requests = 3
};

int mapRequestTypeToGaugeDataOffset(RequestType *type) {
  switch(*type){
    case Instances: return 0;
    case Requests: return 1;
    case CPU_EU: return 2;
    case CPU_US: return 3;
    default: return 0;
  }
}

int convertValueToGaugeValue(int *value, RequestType *type) {
  const int numLEDs = 4;
  int maxValue = 4;
  switch(*type) {
    case Instances:
      maxValue = 4;
      if (*value > maxValue) {
        return maxValue;
      }
      break;
    case CPU_EU:
    case CPU_US:
      maxValue = 100;
      if (*value > maxValue) {
        return maxValue;
      }
      break;
    case Requests:
      maxValue = 150;
      if (*value > maxValue) {
        return maxValue;
      }
      break;
    default: break;
  }
  return numLEDs/maxValue* (*value);
}

byte convertDataToGaugeValue(int *bar1, int *bar2) {
  return (byte) (*bar1 * 16 + *bar2);
}
