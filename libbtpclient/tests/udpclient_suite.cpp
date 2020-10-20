#include <fas/testing.hpp>
#include <btpclient/udpclient.hpp>
namespace {

UNIT(udpclient1, "")
{
  using namespace fas::testing;
  t << nothing;
}

}

BEGIN_SUITE(udpclient, "")
  ADD_UNIT(udpclient1)
END_SUITE(udpclient)
