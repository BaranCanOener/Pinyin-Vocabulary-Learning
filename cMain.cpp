#include "cMain.h"
#include <fstream>
#include <codecvt>
#include <ctime>


wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_TEXT(10001, OnTextInput)
	EVT_BUTTON(10002, OnNewWordClick)
wxEND_EVENT_TABLE()

//Set up UI and read in vocabulary file
cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Pinyin Vocabulary Learning", wxPoint(30,30), wxSize(780,220))
{
	srand(static_cast<int>(time(0)));
	wxFont myFont(14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	this->SetFont(myFont);
	m_input = new wxTextCtrl(this, 10001, "Type here", wxPoint(10, 60), wxSize(200, 60));
	m_output = new wxTextCtrl(this, wxID_ANY, "This box will display the solutions.", wxPoint(220, 60), wxSize(500, 60), wxTE_MULTILINE);
	m_button1 = new wxButton(this, 10002, "New word", wxPoint(10, 130), wxSize(200, 30));
	m_translatePrompt = new wxStaticText(this, wxID_ANY, "", wxPoint(10, 10), wxSize(300, 20));
	m_wordToTranslate = new wxStaticText(this, wxID_ANY, "test", wxPoint(10, 30), wxSize(300, 20));
	myFont = wxFont(14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	m_wordToTranslate->SetFont(myFont);
	m_correctlyTranslated = new wxStaticText(this, wxID_ANY, "Correctly translated: " + std::to_string(m_wordsGuessed), wxPoint(220, 130), wxSize(300, 20));
	ReadVocabularyFile("vocabulary.txt");
	currentPair = select_randomly(vocabulary.begin(), vocabulary.end());
	this->SetBackgroundColour(wxColour(*wxLIGHT_GREY));
	NewWord();
}

cMain::~cMain()
{
}

// Read the specified file into the vocabulary vector
//fileName is assumed to be in UTF-8 format and pairs of words are separated by ';'
//Mandarin word comes before the separator, english word afterwards
//To be implemented: Parsing of several possible translations, e.g. separated by ','
void cMain::ReadVocabularyFile(std::string fileName)
{
	std::wstring line, english, mandarin;
	std::wifstream file(fileName);

	file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	int totalLines = 0;
	while (std::getline(file, line)) {
		mandarin = line.substr(0, line.find(';'));
		line.erase(0, line.find(';') + 1);
		english = line;
		cTranslationPair pair;
		pair.english.push_back(english);
		pair.mandarin.push_back(mandarin);
		vocabulary.push_back(pair);
		totalLines++;
	}
	
}

//Check whether the input textbox control m_input matches the sought after translation
void cMain::ValidateInput()
{
	if ((m_translateToPinyin) && (m_input->GetValue() == currentPair->mandarin.at(0)) || (!m_translateToPinyin) && (m_input->GetValue() == currentPair->english.at(0))) {
		m_wordsGuessed++;
		m_correctlyTranslated->SetLabel("Correctly translated: " + std::to_string(m_wordsGuessed));
		m_output->AppendText("\nCorrect! Translation: \n");
		m_output->AppendText(currentPair->mandarin.at(0) + " - " + currentPair->english.at(0));
		m_output->ShowPosition(m_output->GetLastPosition());
		this->SetBackgroundColour(wxColour(*wxGREEN));
		this->Refresh();
		this->Update();
		currentPair = select_randomly(vocabulary.begin(), vocabulary.end());
		NewWord();
		m_input->ChangeValue("");
		Sleep(50);
		this->SetBackgroundColour(wxColour(*wxLIGHT_GREY));
		this->Refresh();
	}
}

void cMain::NewWord()
{
	//Coin flip to select which language to translate to
	m_translateToPinyin = rand() % 2;
	//Select a word pair using the uniform distribution on the vocabulary vector
	currentPair = select_randomly(vocabulary.begin(), vocabulary.end());
	if (m_translateToPinyin) {
		m_translatePrompt->SetLabel("Translate to Pinyin Chinese:");
		m_wordToTranslate->SetLabel(currentPair->english.at(0));
	}
	else {
		m_translatePrompt->SetLabel("Translate to English:");
		m_wordToTranslate->SetLabel(currentPair->mandarin.at(0));
	}	
	m_wordsGenerated++;
}


void cMain::OnTextInput(wxCommandEvent &evt)
{
	wxString inputStr = m_input->GetValue();
	if (inputStr == "")
		return;
	auto pos = m_input->GetInsertionPoint();
	//Scan string for vowels followed by tone numerals and replace accordingly
	for (auto it = inputStr.begin() + 1; it != inputStr.end(); ++it) {
		auto wch = *it;
		auto wchPrev = *(it - 1);

		//Lambda function to replace the character at it with newChar, and delete the subsequent character
		auto replace = [&](wxUniChar newChar) {
			inputStr.erase(it);
			inputStr.erase(it - 1);
			inputStr.insert(it - 1, newChar);
			it--;
		};

		//Tone number 1 corresponding to macron
		if (wch == '1') {
			switch ((char)wchPrev) {
			case 'a':
				replace(wxUniChar(257));
				break;
			case 'A':
				replace(wxUniChar(256));
				break;
			case 'e':
				replace(wxUniChar(275));
				break;
			case 'E':
				replace(wxUniChar(274));
				break;
			case 'i':
				replace(wxUniChar(299));
				break;
			case 'I':
				replace(wxUniChar(298));
				break;
			case 'o':
				replace(wxUniChar(333));
				break;
			case 'O':
				replace(wxUniChar(332));
				break;
			case 'u':
				replace(wxUniChar(363));
				break;
			case 'U':
				replace(wxUniChar(362));
				break;
			}
		}
		//Tone number 2 corresponding to acute accent
		else if (wch == '2') {
			switch ((char)wchPrev) {
			case 'a':
				replace(wxUniChar(225));
				break;
			case 'A':
				replace(wxUniChar(193));
				break;
			case 'e':
				replace(wxUniChar(233));
				break;
			case 'E':
				replace(wxUniChar(201));
				break;
			case 'i':
				replace(wxUniChar(237));
				break;
			case 'I':
				replace(wxUniChar(205));
				break;
			case 'o':
				replace(wxUniChar(243));
				break;
			case 'O':
				replace(wxUniChar(336));
				break;
			case 'u':
				replace(wxUniChar(250));
				break;
			case 'U':
				replace(wxUniChar(368));
				break;
			}
		}
		//Tone number 3 corresponding to caron
		else if (wch == '3') {
			switch ((char)wchPrev) {
			case 'a':
				replace(wxUniChar(462));
				break;
			case 'A':
				replace(wxUniChar(461));
				break;
			case 'e':
				replace(wxUniChar(283));
				break;
			case 'E':
				replace(wxUniChar(282));
				break;
			case 'i':
				replace(wxUniChar(464));
				break;
			case 'I':
				replace(wxUniChar(463));
				break;
			case 'o':
				replace(wxUniChar(466));
				break;
			case 'O':
				replace(wxUniChar(465));
				break;
			case 'u':
				replace(wxUniChar(468));
				break;
			case 'U':
				replace(wxUniChar(467));
				break;
			}
		}
		//Tone number 4 corresponding to grave accent
		else if (wch == '4') {
		switch ((char)wchPrev) {
		case 'a':
			replace(wxUniChar(224));
			break;
		case 'A':
			replace(wxUniChar(192));
			break;
		case 'e':
			replace(wxUniChar(232));
			break;
		case 'E':
			replace(wxUniChar(200));
			break;
		case 'i':
			replace(wxUniChar(236));
			break;
		case 'I':
			replace(wxUniChar(204));
			break;
		case 'o':
			replace(wxUniChar(242));
			break;
		case 'O':
			replace(wxUniChar(210));
			break;
		case 'u':
			replace(wxUniChar(249));
			break;
		case 'U':
			replace(wxUniChar(217));
			break;
		}
		}
	}
	//Call ChangeValue rather than SetValue to avoid buffer overflow
	m_input->ChangeValue(inputStr);
	m_input->SetInsertionPoint(pos);
	ValidateInput();
}

void cMain::OnNewWordClick(wxCommandEvent& evt)
{
	if (m_wordsGenerated > 0) {
		//Show what the translation of the old word would have been
		m_output->AppendText("\nThe translation would have been: \n");
		m_output->AppendText(currentPair->mandarin.at(0) + " - " + currentPair->english.at(0));
	}
	NewWord();
}
