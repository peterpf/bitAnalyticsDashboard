int convertValueToGaugeValue(double *value, RequestType *type) {
  const int numLEDs = 4;
  double maxValue = 4.0;
  int result = *value;
  switch(*type) {
    case Instances:
      maxValue = NUM_MAX_INSTANCES;
      if (*value > maxValue) {
        result = numLEDs;
      }
      break;
    case CPU_EU:
    case CPU_US:
      maxValue = NUM_MAX_CPU_USAGE;
      result = *value * 100;
      if (result > maxValue) {
        result = numLEDs;
      }
      break;
    case Requests:
      maxValue = NUM_MAX_REQUESTS;
      if (*value > maxValue) {
        result = numLEDs;
      }
      break;
    default: break;
  }
  const int numIluminatedLEDs = round(numLEDs/maxValue * result);
  return pow(2, numIluminatedLEDs)-1;
}

byte convertDataToGaugeValue(int *bar1, int *bar2) {
  return (byte) (*bar1 * 16 + *bar2);
}
