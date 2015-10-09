#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#import <msxml3.dll>
//#include <msxml2.h>
#include <vector>
#include <list>
#include <unordered_map>
#include "tstring.h"
#include <comutil.h>
#include <fstream>
#include <memory>

//#include "ofApp.h"

using namespace MSXML2;
using namespace std;


struct XmlNode
{
    tstring nodeName;
    unordered_map<tstring, tstring> attibutesMap;
    tstring text;
    list<shared_ptr<XmlNode>> nodesVec;
};

class Hook {
		      
		public:
			std::string text;
			std::string hookfont;
			Hook();
			Hook(string text1, string hookfont1);
			Hook(string landmark);
			Hook::Hook(int i);
		         
};


tstring MyConvertBSTRToTstring(BSTR bStr);

unordered_map<tstring, tstring> GetNodeAttributes(MSXML2::IXMLDOMNode *pIDOMNode);

list<shared_ptr<XmlNode>> GetChildNodeCollection(MSXML2::IXMLDOMNode *pIDOMNode);

shared_ptr<XmlNode> GetNodeCollection(MSXML2::IXMLDOMDocumentPtr docPtr, TCHAR* nodeVal);

vector<Hook> GetData();


