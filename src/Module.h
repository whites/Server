#ifndef MODULE_H_
#define MODULE_H_

class Module
{
public:
    virtual bool moduleInit() { return true; };
    virtual bool moduleDestroy() { return true; };

};

#endif /* MODULE_H_ */