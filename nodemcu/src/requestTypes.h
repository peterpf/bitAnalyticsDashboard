enum RequestType {
  Instances = 0,
  Requests  = 1,
  CPU_EU    = 2,
  CPU_US    = 3,
};

int getNumResponseValuesForRequestType(RequestType *type) {
  switch(*type) {
    case Instances:
    case Requests:
      return 2;
    default:
      return 1;
  }
}
