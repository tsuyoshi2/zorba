
#include "zorbaserialization/class_serializer.h"
#include "zorbaserialization/archiver.h"
#include "zorbaserialization/base64impl.h"

#include "zorbaerrors/error_manager.h"

namespace zorba{
  namespace serialization{

extern ClassSerializer  *g_class_serializer = NULL;
ClassSerializerDestroyer g_class_serializer_destroyer;

//////////////////////////////////////////////
///////////////////////////////////////////Global serialization Operators
ClassSerializer::ClassSerializer()
{
  class_name_pool_size = 10000;
  class_name_pool = (char*)malloc(class_name_pool_size);
  class_name_pool_filled = 0;

  t0 = clock();
  g_class_serializer_destroyer.activate();
}

ClassSerializer::~ClassSerializer()
{
  free(class_name_pool);
}

ClassSerializer* ClassSerializer::getInstance()
{
  if(!g_class_serializer)
    g_class_serializer = new ClassSerializer();
  return g_class_serializer;
}

void ClassSerializer::deleteInstance()
{
  delete g_class_serializer;
  g_class_serializer = NULL;
}

void ClassSerializer::register_class_factory(const char *class_name, class_deserializer *class_factory)
{
#ifdef DEBUG
  //check for class name duplicates
  if(get_class_factory(class_name))
  {
    assert(false);
  }
#endif
  int name_size = strlen(class_name);
  if((name_size + class_name_pool_filled + 1) > class_name_pool_size)
  {
    class_name_pool_size += 10000;
    class_name_pool = (char*)realloc(class_name_pool, class_name_pool_size);
  }
//  std::cout << "register class factory " << class_name << std::endl;
  class_factories.resize(class_factories.size()+1);
  ClassSerializer::registered_factory  &r = class_factories.back();
  //strcpy(r.class_name, class_name);
  strcpy(class_name_pool + class_name_pool_filled, class_name);
  r.cls_name_off = class_name_pool_filled;
  class_name_pool_filled += name_size + 1;
  r.class_factory = class_factory;

  t1 = clock();
}

class_deserializer *ClassSerializer::get_class_factory(const char *classname)
{
  std::list<struct registered_factory>::iterator  it;
  for(it = class_factories.begin(); it != class_factories.end(); it++)
  {
    if(!strcmp(class_name_pool + (*it).cls_name_off, classname))
      return (*it).class_factory;
  }
  return NULL;
}


//////////////////////////////////////////////
///////////////////////////////////////////Global serialization Operators
void operator&(Archiver &ar, int &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%d", obj);

    ar.add_simple_field("int", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "int", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%d", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, uint32_t &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%u", obj);

    ar.add_simple_field("uint32_t", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "uint32_t", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%u", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, long &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%ld", obj);

    ar.add_simple_field("long", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "long", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%ld", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, long long &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%lld", obj);

    ar.add_simple_field("long long", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "long long", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%lld", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, unsigned long long &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%llu", obj);

    ar.add_simple_field("unsigned long long", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "unsigned long long", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%llu", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, short &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%hd", obj);

    ar.add_simple_field("short", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "short", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%hd", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, unsigned short &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%hu", obj);

    ar.add_simple_field("unsigned short", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "unsigned short", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%hu", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, char &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%c", obj);

    ar.add_simple_field("char", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "char", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%c", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, unsigned char &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%c", obj);

    ar.add_simple_field("unsigned char", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "unsigned char", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    char ctemp;
    sscanf(value.c_str(), "%c", &ctemp);
    obj = (unsigned char)ctemp;

    ar.register_reference(id, field_treat, &obj);
  }
}


void operator&(Archiver &ar, float &obj)
{
  if(ar.is_serializing_out())
  {
    char  strtemp[30];
    sprintf(strtemp, "%f", obj);

    ar.add_simple_field("float", strtemp, &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "float", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    sscanf(value.c_str(), "%f", &obj);

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, bool &obj)
{
  if(ar.is_serializing_out())
  {
    ar.add_simple_field("bool", obj ? "true" : "false", &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "bool", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    if(value == "true")
    {
      obj = true;
    }
    else if(value == "false")
    {
      obj = false;
    }
    else
    {
      ZORBA_ERROR_DESC_OSS(SRL0002_INCOMPATIBLE_INPUT_FIELD, id);
    }

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, std::string &obj)
{
  if(ar.is_serializing_out())
  {
    ar.add_simple_field("string", obj.c_str(), &obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "string", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);
    obj = value;

    ar.register_reference(id, field_treat, &obj);
  }
}

void operator&(Archiver &ar, char* &obj)//like char *p=strdup("io");
{
  if(ar.is_serializing_out())
  {
    ar.add_simple_field("char*", obj, obj, obj ? ARCHIVE_FIELD_IS_PTR : ARCHIVE_FIELD_IS_NULL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "char*", is_simple, (ArchiveFieldTreat)0, (ArchiveFieldTreat)0, id);
    if(field_treat == ARCHIVE_FIELD_IS_NULL)
    {
      obj = NULL;
      return;
    }
    if((field_treat != ARCHIVE_FIELD_IS_PTR) && (field_treat != ARCHIVE_FIELD_IS_REFERENCING))
    {
      ZORBA_ERROR_DESC_OSS(SRL0002_INCOMPATIBLE_INPUT_FIELD, id);
    }
    if(field_treat == ARCHIVE_FIELD_IS_PTR)
    {
      obj = _strdup(value.c_str());
      ar.register_reference(id, field_treat, obj);
    }
    else if(id > referencing)// ARCHIVE_FIELD_IS_REFERENCING
      obj = (char*)ar.get_reference_value(referencing);
    else
      ar.register_delay_reference((void**)&obj, !FIELD_IS_CLASS, NULL, referencing);
  }
}

void serialize_array(Archiver &ar, unsigned char *obj, int len)//like char p[20]
{
  if(ar.is_serializing_out())
  {
    char  *base64string;
    base64string = (char*)malloc(8*len/6 + 2);
    Base64Impl::Encode(obj, len, base64string);
    ar.add_simple_field("char[]", base64string, obj, ARCHIVE_FIELD_NORMAL);
  }
  else
  {
    char  *type;
    std::string value;
    int   id;
    int   version;
    bool  is_simple;
    bool  is_class;
    enum  ArchiveFieldTreat field_treat;
    int   referencing;
    bool  retval;
    retval = ar.read_next_field(&type, &value, &id, &version, &is_simple, &is_class, &field_treat, &referencing);
    if(!retval && ar.get_read_optional_field())
      return;
    ar.check_simple_field(retval, type, "char[]", is_simple, field_treat, ARCHIVE_FIELD_NORMAL, id);

    Base64Impl::Decode((const unsigned char*)value.c_str(), value.length(), obj);
    ar.register_reference(id, field_treat, &obj);
  }
}


void report_error(   XQUERY_ERROR        aErrorCode,
                      const std::string  aDesc, 
                      const std::string   aFileName,
                      int                 aLineNumber)
{
  throw error::ErrorManager::createException(aErrorCode,                    
                                             aDesc,    
                                             aFileName, aLineNumber);     
}

}}
