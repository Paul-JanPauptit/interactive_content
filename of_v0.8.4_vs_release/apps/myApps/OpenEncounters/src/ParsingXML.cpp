#include "windows.h"
#include "ParsingXML.h"



#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

void XMLDOMParsingNodes(char*);

Hook::Hook()
{
	text = "Would you befriend an elf? ";
	hookfont = "COPRGTB.ttf";
}

Hook::Hook(string text1, string hookfont1)

{
	text = text;
	hookfont = hookfont1;
}

Hook::Hook(string landmark)

{
	text = landmark + " Would you befriend an elf? ";
	hookfont = "COPRGTB.ttf";
}


tstring MyConvertBSTRToTstring(BSTR bStr)
{
    UINT length = SysStringLen(bStr);
    wchar_t *myString = new wchar_t[length+1];
    wcscpy(myString, bStr);
    tstring convertedString(myString);
    delete myString;
    return convertedString;
}

unordered_map<tstring, tstring> GetNodeAttributes(MSXML2::IXMLDOMNode *pIDOMNode)
{
    BSTR bstrAttrName, bstrAttrText, bstrNodeType;
    MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;
    MSXML2::IXMLDOMNode *pIAttrNode = NULL;
    HRESULT hResult;
    unordered_map<tstring, tstring> info;

    if(pIDOMNode )
    {
        pIDOMNode->get_nodeTypeString(&bstrNodeType);
        BSTR temp = L"element";

        if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)temp)==0) 
        {
            int j = 0;
            long length;

            DOMNamedNodeMapPtr = pIDOMNode->attributes;
            hResult = DOMNamedNodeMapPtr->get_length(&length);

            if(SUCCEEDED(hResult))
            {
                for(j = 0; j < length; j++)
                {
                    DOMNamedNodeMapPtr->get_item(j,&pIAttrNode);

                    pIAttrNode->get_nodeName(&bstrAttrName);
                    pIAttrNode->get_text(&bstrAttrText);

                    tstring strAttrName = MyConvertBSTRToTstring(bstrAttrName);
                    tstring strAttrText = MyConvertBSTRToTstring(bstrAttrText);
                    info.insert(make_pair(strAttrName, strAttrText));
                    pIAttrNode->Release();
                    pIAttrNode = NULL;
                }
            }
        }
    }
    return info;
}

list<shared_ptr<XmlNode>> GetChildNodeCollection(MSXML2::IXMLDOMNode *pIDOMNode)
{
    MSXML2::IXMLDOMNode *pIChildNode = NULL;
    shared_ptr<XmlNode> node;
    list<shared_ptr<XmlNode>> nodeCollection;

    if(pIDOMNode)
    {
        MSXML2::IXMLDOMNodeListPtr Childs = pIDOMNode->childNodes;
        for(int i = 0; i < (Childs->length); i++)
        {
            Childs->get_item(i, &pIChildNode);

            node = shared_ptr<XmlNode>(new XmlNode);
            node->nodeName = pIChildNode->GetnodeName();
            node->attibutesMap = GetNodeAttributes(pIChildNode);
            node->text = pIChildNode->Gettext();
            node->nodesVec = GetChildNodeCollection(pIChildNode);

            nodeCollection.push_back(node);
            node = NULL;
            pIChildNode->Release();
            pIChildNode = NULL;
        }
    }
    return nodeCollection;
}

shared_ptr<XmlNode> GetNodeCollection(MSXML2::IXMLDOMDocumentPtr docPtr, TCHAR* nodeVal)
{
    MSXML2::IXMLDOMNodeListPtr NodeListPtr;
    MSXML2::IXMLDOMNode *pIDOMNode = NULL;

    shared_ptr<XmlNode> nodeCollection(new XmlNode);
    int i = 0 ,n = 0;
    NodeListPtr = docPtr->getElementsByTagName(nodeVal);
    NodeListPtr->get_item(0, &pIDOMNode);
    nodeCollection->nodeName = nodeVal;
    if(pIDOMNode)
    {
        nodeCollection->attibutesMap = GetNodeAttributes(pIDOMNode);
        nodeCollection->text = pIDOMNode->Gettext();
        nodeCollection->nodesVec = GetChildNodeCollection(pIDOMNode);

        pIDOMNode->Release();
        pIDOMNode = NULL;
    }
    return nodeCollection;
}

vector<Hook> GetData(string filename)
{
	// Exception must be handled properly 

	vector<Hook> Hookvector;

	const char* msg= filename.c_str(); //use const char*, instead of char*
	wchar_t *wmsg = new wchar_t[strlen(msg)+1]; //memory allocation
	mbstowcs(wmsg, msg, strlen(msg)+1);

	//then use wmsg instead of msg
	//_tcscpy(cmdline, wmsg);

	//memory deallocation - must do to avoid memory leak!

	//TCHAR szFileName[100] = _T("C:\\Users\\nikhil\\Desktop\\of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\BaseHooks.xml");

	TCHAR szFileName[200];

	_tcscpy(szFileName, wmsg);

	wmsg = nullptr;

	delete []wmsg;

    try
    {
        CoInitialize(NULL);
        MSXML2::IXMLDOMDocumentPtr docPtr;
        HRESULT hr = docPtr.CreateInstance(__uuidof(DOMDocument30));
        if(FAILED(hr))
        {
            MessageBox(0, _T("Failed to create the XML class instance"), \
                _T("Create Instance of DOMDoc Ptr"), MB_OK |MB_ICONWARNING);
			return Hookvector;
        }

        _variant_t varXml(szFileName);
        _variant_t varResult((bool)TRUE);

        varResult = docPtr->load(varXml);

        if ((bool)varResult == FALSE)
        {
            //printf("*** Error:failed to load XML file. ***\n");
            MessageBox(0,_T("Error: failed to load XML file. Check the file name."), \
                _T("Load XML file"), MB_OK |MB_ICONWARNING);
			return Hookvector;
        }

        shared_ptr<XmlNode> batteryNode = GetNodeCollection(docPtr, _T("Filtered_hooks"));

		XmlNode* node_ptr = batteryNode.get();
		
		for (auto var : batteryNode->nodesVec)
		{
			list<shared_ptr<XmlNode>> node = var->nodesVec;
			Hook HookfromNode;

			for (auto v : node)
			{
				tstring text = v->text;
				tstring font;
				tstring Nodename = v->nodeName;
				//v->nodesVec;
				wchar_t* t = L"text";
				wchar_t* f = L"font";
				wstring txt(t);
				wstring fnt(f);
						

				if (Nodename.compare(txt) == 0)
				{
					string value (text.begin(), text.end() );
					HookfromNode.text = value;
					
				}
				// Uncomment this when XML file is ready with hooks

				if (Nodename.compare(fnt) == 0)
				{
					string value (text.begin(), text.end() );
					HookfromNode.hookfont = value;
				}
																		
			}

			Hookvector.push_back(HookfromNode);
						
		}
		return Hookvector;

	}
	
    catch(exception ex)
    {
        MessageBox(NULL, _T("*** Exception occurred ***"), _T("Error message"), MB_OK);
    }

    CoUninitialize();

	
}



//
//
//int main(int argc, char* argv[])
//{
//    GetData();
//    return 0;
//}
