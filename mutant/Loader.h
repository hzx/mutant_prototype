#ifndef LOADER_H
#define LOADER_H


class Loader {
public:
  Loader();
  ~Loader();
  void setPaths(char *paths);
  void loadModule(const char *name);
  void loadSources(const char *folder);

private:
  /* data */
};


#endif /* end of include guard: LOADER_H */
