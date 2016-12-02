const static char CVSId[] =
  "$Id$";

#include "corsika_test.h"

#include <corsika/RawStream.h>
#include <corsika/RawParticleIterator.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace corsika;


TEST_GROUP(CorsikaTest);

namespace testRawStreamNS {
  void test_copy(string filename)
  {
      boost::shared_ptr<FileStream> f(FileStream::open(filename.c_str()));

    corsika::RawStream<corsika::NotThinned> rawUnthinnedStream(f, filename);

    Block<corsika::NotThinned> block;
    rawUnthinnedStream.GetNextBlock(block);
    rawUnthinnedStream.GetNextBlock(block);
  }

  void test_basic(string filename)
  {
      boost::shared_ptr<FileStream> f(FileStream::open(filename.c_str()));
    boost::shared_ptr<corsika::RawStream<corsika::NotThinned> > rawUnthinnedStream(new corsika::RawStream<corsika::NotThinned>(f, filename));

    Block<corsika::NotThinned> block;
    rawUnthinnedStream->GetNextBlock(block);
    ENSURE(block.IsRunHeader());

    rawUnthinnedStream->GetNextBlock(block);
    ENSURE(block.IsEventHeader());

    ENSURE(rawUnthinnedStream->DiskBlockBuffer().fPaddingBeginning[0] == corsika::NotThinned::kBytesPerBlock);
    ENSURE(rawUnthinnedStream->DiskBlockBuffer().fPaddingEnd[0] == corsika::NotThinned::kBytesPerBlock);

    int i = 0;
    while (rawUnthinnedStream->GetNextBlock(block) && i < 5000) {
      i += 1;
    }
    ENSURE_EQUAL(i, 4723);

    i = 0;
    int all = 0;
    boost::shared_ptr<VRawParticleIterator> it = rawUnthinnedStream->GetVParticleIt(2);
    boost::optional<CorsikaParticle> p = it->GetCorsikaParticle();
    while (p) {
      ++all;
      if (p->fDescription > 0 &&
          p->CorsikaCode() != 75 &&  p->CorsikaCode() != 76 &&
          p->CorsikaCode() != 85 &&  p->CorsikaCode() != 86 &&
          p->CorsikaCode() != 95 &&  p->CorsikaCode() != 96)
      i += 1;
      p = it->GetCorsikaParticle();
    }
    ENSURE_EQUAL(all, 183339);
    ENSURE_EQUAL(i, 181992);
  }


}

using namespace testRawStreamNS;

TEST(testRawStream)
{
  string dir(CORSIKA_TEST_DIR);
  test_copy(dir + string("/DAT000002-32"));
  vector<string> filenames;
  filenames.push_back("/DAT000002-32");

  for (unsigned int i = 0; i != filenames.size(); ++i) {
    std::cout << "testing raw stream " << filenames[i] << std::endl;
    test_basic(dir + filenames[i]);
  }
  //ENSURE(Verify<CloseTo>(p.GetCoordinates(CTrans), Triple(-1,0,0)));
}





// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// End:
