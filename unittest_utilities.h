
// $ cxxtestgen.py --error-printer -o test_runner.cpp unittest_utilities.h 
// $ g++ -o test_runner test_runner.cpp utilities.cpp -I/usr/local/include

#include <cxxtest/TestSuite.h>

#include <string>
using namespace std;
#include "utilities_pub.h"


class NewTestSuite : public CxxTest::TestSuite 
{
public:

  // Tests for SplitString()
  void testSplitString_Basic( void )
  {
    string  s1 = "two words";
    vector<string>  s1_split;
    vector<string>  correctResult12;
    correctResult12.push_back("two");
    correctResult12.push_back("words");
    SplitString(s1, s1_split);
    TS_ASSERT( s1_split == correctResult12 );

    string  s2 = "   two words    ";
    vector<string>  s2_split;
    SplitString(s2, s2_split);
    TS_ASSERT( s2_split == correctResult12 );

    string  s3 = "\tthree words\t now";
    vector<string>  s3_split;
    vector<string>  correctResult3;
    correctResult3.push_back("three");
    correctResult3.push_back("words");
    correctResult3.push_back("now");
    SplitString(s3, s3_split);
    TS_ASSERT( s3_split == correctResult3 );
  }

  void testSplitString_csv( void )
  {
    string  s1 = "two,words";
    vector<string>  s1_split;
    vector<string>  correctResult12;
    correctResult12.push_back("two");
    correctResult12.push_back("words");
    SplitString(s1, s1_split, ",");
    TS_ASSERT( s1_split == correctResult12 );
  }

  void testSplitString_storage( void )
  // test to make sure that SplitString() clears the supplied vector
  // before storing things in it
  {
    string  s1 = "two words";
    string  s2 = "more words";
    vector<string>  s_split;
    vector<string>  correctResult;
    correctResult.push_back("more");
    correctResult.push_back("words");
    SplitString(s1, s_split);
    SplitString(s2, s_split);
    TS_ASSERT( s_split == correctResult );
  }
  

  void testSplitStringAdd_storage( void )
  // test to make sure that SplitStringAdd() does *not* clear the supplied vector
  // before storing things in it
  {
    string  s1 = "two words";
    string  s2 = "more words";
    vector<string>  s_split;
    vector<string>  correctResult;
    correctResult.push_back("two");
    correctResult.push_back("words");
    correctResult.push_back("more");
    correctResult.push_back("words");
    SplitStringAdd(s1, s_split);
    SplitStringAdd(s2, s_split);
    TS_ASSERT( s_split == correctResult );
  }
  

  // Tests for TrimWhitespace()
  void testTrimWhitespace( void )
  {
    string  s1 = "\t\ttext with tabs\t";
    string  correctResult1 = "text with tabs";
    string  s2 = "   singleword";
    string  correctResult2 = "singleword";
    string  s3 = "\tsingleword";
    string  correctResult3 = "singleword";
    string  s4 = "singleword\t";
    string  correctResult4 = "singleword";
    TrimWhitespace(s1);
    TS_ASSERT( s1 == correctResult1 );
    TrimWhitespace(s2);
    TS_ASSERT( s2 == correctResult2 );
    TrimWhitespace(s3);
    TS_ASSERT( s3 == correctResult3 );
    TrimWhitespace(s4);
    TS_ASSERT( s4 == correctResult4 );
  }


  // Tests for ChopComment()
  void testChopComment_Standard( void )
  {
    string  s1 = "text # with comment";
    string  correctResult1 = "text ";
    string  s2 = "#text # with comment";
    string  correctResult2 = "";
    ChopComment(s1);
    TS_ASSERT( s1 == correctResult1 );
    ChopComment(s2);
    TS_ASSERT( s2 == correctResult2 );
  }

  void testChopComment_Alternate( void )
  {
    string  s1 = "text ; with comment";
    string  correctResult1 = "text ";
    string  s2 = ";text # with comment";
    string  correctResult2 = "";
    ChopComment(s1, ';');
    TS_ASSERT( s1 == correctResult1 );
    ChopComment(s2, ';');
    TS_ASSERT( s2 == correctResult2 );
  }
  
  
  // Tests for ImageFileExists()
  void testImageFileExists_internet( void )
  {
    TS_ASSERT( ImageFileExists("ftp://www.blah.com/something") == true);
    TS_ASSERT( ImageFileExists("http://www.blah.com/something") == true);
  }

  void testImageFileExists_NonexistentFile( void )
  {
    TS_ASSERT( ImageFileExists("/Users/erwin/coding/imfit/testing/nohow.fits") == false);
  }
  
  void testImageFileExists_RealFile( void )
  {
    TS_ASSERT( ImageFileExists("/Users/erwin/coding/imfit/testing/n3073rss_small.fits") == true);
  }
  
  void testImageFileExists_RealFileWithSection( void )
  {
    TS_ASSERT( ImageFileExists("/Users/erwin/coding/imfit/testing/n3073rss_small.fits[100:200,200:300]") == true);
  }
  
  
  // Tests for FileExists()
  void testFileExists_RealFile( void )
  {
    TS_ASSERT( FileExists("/Users/erwin/coding/testing/thread1.c") == true);
  }
   
  void testFileExists_NonexistentFile( void )
  {
    TS_ASSERT( FileExists("/Users/erwin/coding/testing/nobody-nohow.dat") == false);
  }


  // Tests for StripBrackets()
  void testStripBrackets_NormalFiles( void )
  {
    string  s1_out, s2_out;
    string  s1 = "image.fits";
    string  correctResult1 = "image.fits";
    string  s2 = "image.fit";
    string  correctResult2 = "image.fit";
    StripBrackets(s1, s1_out);
    TS_ASSERT( s1_out == correctResult1 );
    StripBrackets(s2, s2_out);
    TS_ASSERT( s2_out == correctResult2 );
  }

  void testStripBrackets_WithBrackets( void )
  {
    string  s1_out, s2_out;
    string  s1 = "image.fits[100:200,300:400]";
    string  correctResult1 = "image.fits";
    string  s2 = "image.fit[100:200,300:400]";
    string  correctResult2 = "image.fit";
    StripBrackets(s1, s1_out);
    TS_ASSERT( s1_out == correctResult1 );
    StripBrackets(s2, s2_out);
    TS_ASSERT( s2_out == correctResult2 );
  }



  // Tests for GetPixelStartCoords()
  void testGetPixelStartCoords_NoSection( void )
  {
    string  s1("image.fits");
    string  s2("complicated_name_image.fit");
    int  x1, y1, x2, y2;
    int  correct_xoff = 1;
    int  correct_yoff = 1;
    GetPixelStartCoords(s1, &x1, &y1);
    TS_ASSERT( x1 == correct_xoff );
    TS_ASSERT( y1 == correct_yoff );
    GetPixelStartCoords(s2, &x2, &y2);
    TS_ASSERT( x2 == correct_xoff );
    TS_ASSERT( y2 == correct_yoff );
  }

  void testGetPixelStartCoords_BadSections( void )
  {
    string  s1("image.fits[100:200,200:400");
    string  s2("complicated_name_image.fit[100:200,200:400[");
    string  s3("image.fits[100:200]");
    int  x1, y1, x2, y2, x3, y3;
    int  correct_xoff = 0;
    int  correct_yoff = 0;
    GetPixelStartCoords(s1, &x1, &y1);
    TS_ASSERT( x1 == correct_xoff );
    TS_ASSERT( y1 == correct_yoff );
    GetPixelStartCoords(s2, &x2, &y2);
    TS_ASSERT( x2 == correct_xoff );
    TS_ASSERT( y2 == correct_yoff );
    GetPixelStartCoords(s3, &x3, &y3);
    TS_ASSERT( x3 == correct_xoff );
    TS_ASSERT( y3 == correct_yoff );
  }

  void testGetPixelStartCoords_BasicSections( void )
  {
    string  s1("image.fits[100:200,200:400]");
    string  s2("complicated_name_image.fit[100:200,200:400]");
    int  x1, y1, x2, y2;
    int  correct_xoff = 100;
    int  correct_yoff = 200;
    GetPixelStartCoords(s1, &x1, &y1);
    TS_ASSERT( x1 == correct_xoff );
    TS_ASSERT( y1 == correct_yoff );
    GetPixelStartCoords(s2, &x2, &y2);
    TS_ASSERT( x2 == correct_xoff );
    TS_ASSERT( y2 == correct_yoff );
  }

  void testGetPixelStartCoords_StarredSections( void )
  {
    string  s1("image.fits[*,200:400]");
    string  s2("image.fits[100:200,*]");
    int  x1, y1, x2, y2;
    int  correct_xoff1 = 1;
    int  correct_yoff1 = 200;
    int  correct_xoff2 = 100;
    int  correct_yoff2 = 1;
    GetPixelStartCoords(s1, &x1, &y1);
    TS_ASSERT( x1 == correct_xoff1 );
    TS_ASSERT( y1 == correct_yoff1 );
    GetPixelStartCoords(s2, &x2, &y2);
    TS_ASSERT( x2 == correct_xoff2 );
    TS_ASSERT( y2 == correct_yoff2 );
  }



  
  // Tests for NotANumber()
  void testNotANumberWithInts( void )
  {
    bool  t1 = NotANumber( "5", 0, kAnyInt );
    TS_ASSERT( t1 == false);
    bool  t2 = NotANumber( "-5", 1, kAnyInt );
    TS_ASSERT( t2 == false);
    bool  t3 = NotANumber( "5", 0, kPosInt );
    TS_ASSERT( t3 == false);
    // Now test that 0 is "not a number" for kPosInt
    bool  t4 = NotANumber( "0", 0, kPosInt );
    TS_ASSERT( t4 == true);
    // Now test that a is "not a number" for kPosInt
    bool  t5 = NotANumber( "a", 0, kPosInt );
    TS_ASSERT( t5 == true);
    // Now test that ".1" is "not a number" for kPosInt
    bool  t6 = NotANumber( ".1", 0, kPosInt );
    TS_ASSERT( t6 == true);
  }

  void testNotANumberWithReals( void )
  {
    // Tests for kAnyReal
    bool  t1 = NotANumber( "5.7", 0, kAnyReal );
    TS_ASSERT( t1 == false);
    bool  t2 = NotANumber( "-5.7", 1, kAnyReal );
    TS_ASSERT( t2 == false);
    bool  t3 = NotANumber( ".57", 0, kAnyReal );
    TS_ASSERT( t3 == false);
    bool  t3b = NotANumber( "0", 0, kAnyReal );
    TS_ASSERT( t3b == false);
    bool  t4 = NotANumber( "-.57", 0, kAnyReal );
    TS_ASSERT( t4 == false);
    // Now test that a is "not a number" for kAnyReal
    bool  t5 = NotANumber( "a", 0, kAnyReal );
    TS_ASSERT( t5 == true);
    // Tests for kPosReal
    bool  t6 = NotANumber( "5.7", 0, kPosReal );
    TS_ASSERT( t6 == false);
    bool  t7 = NotANumber( ".57", 0, kPosReal );
    TS_ASSERT( t7 == false);
    // Now test for "not a number" for kPosReal
    bool  t8 = NotANumber( "a", 0, kPosReal );
    TS_ASSERT( t8 == true);
    bool  t9 = NotANumber( "-.57", 0, kPosReal );
    TS_ASSERT( t9 == true);
    bool  t10 = NotANumber( "-0.2", 0, kPosReal );
    TS_ASSERT( t10 == true);
    // THE FOLLOWING DOES NOT WORK -- APPARENTLY RETURNS FALSE!
    bool  t11 = NotANumber( "-5.7", 0, kPosReal );
    TS_ASSERT( t11 == true);
  }
};
