#pragma once

#include "wx/wx.h"
#include  <random>
#include  <iterator>

//Corresponds to a single pair of english and mandarin words (saved as a vector to accomodate multiple translations - not implemented yet)
struct cTranslationPair
{
	std::vector<std::wstring> mandarin;
	std::vector<std::wstring> english;
};

//cf. https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();
public:
	std::vector<cTranslationPair> vocabulary;
	std::vector<cTranslationPair>::iterator currentPair;
	void ReadVocabularyFile(std::string fileName);
	void ValidateInput();
	void NewWord();
	bool m_translateToPinyin;
	int m_wordsGuessed = 0;
	int m_wordsGenerated = 0;
public:
	wxTextCtrl* m_input = nullptr;
	wxTextCtrl* m_output = nullptr;
	wxButton* m_button1 = nullptr;
	wxStaticText* m_translatePrompt = nullptr;
	wxStaticText* m_wordToTranslate = nullptr;
	wxStaticText* m_correctlyTranslated = nullptr;
	void OnTextInput(wxCommandEvent& evt);
	void OnNewWordClick(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();
};

