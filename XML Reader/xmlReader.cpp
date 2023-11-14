#include <iostream>
#include <fstream> 
#include <string>

using namespace std;

ifstream fileIn;
ofstream fileOut;

template<typename T>
class Node
{
private:
	T data;
	Node<T>* next;

public:

	Node() { next = nullptr; }

	Node(T data, Node<T>* next = nullptr) : data(data), next(next) {}

	T getData() { return data; }
	void setData(const T& data) { this->data = data; }

	Node<T>* getNext() { return next; }
	void setNext(Node<T>* next) { this->next = next; }

};

template <typename T>
class Stack
{
private:
	Node<T>* head;
	int size;

public:

	Stack() : head(new Node<T>), size(0) {}

	void push(const T& data)
	{
		Node<T>* newNode = new Node<T>(data, head->getNext());
		head->setNext(newNode);
		size++;
	}

	T pop()
	{
		T element = head->getNext()->getData();

		Node<T>* popThis = head->getNext();

		head->setNext(popThis->getNext());
		delete popThis;
		popThis = nullptr;

		return element;
	}

	T getTop()
	{
		return head->getNext()->getData();
	}

	bool isEmpty()
	{
		if (head->getNext()) return false;
		return true;
	}

};


class XMLData
{
public:
	string tag;
	Stack<string> name;
	Stack<string> value;
	int line;
	bool text;

	XMLData() {}
	XMLData(int line) : line(line), text(true) {}
	XMLData(int line, string tag) : line(line), tag(tag), text(true) {}

	void setLine(int line) { this->line = line; }
	int getLine() { return this->line; }

	void setName(string tag) { this->tag = tag; }
	string getName() { return this->tag; }

	void setText(bool text) { this->text = text; }
	bool getText() { return this->text; }

	void newAttribute(const string& name, const string& value)
	{
		this->name.push(name);
		this->value.push(value);
	}

	friend ostream& operator << (ostream& os, const XMLData& obj)
	{
		cout << "Line: " << obj.line << "\t" << obj.tag;
		return os;
	}

};


bool readFile(const string& fileName)
{
	Stack<XMLData> tags;
	Stack<XMLData> errors;
	bool prolog = false;

	fileIn.open(fileName);

	string currentLine = "";
	int line = 0, index = 0;


	while (!fileIn.eof()) //read the whole file (line by line)
	{
		line++;

		//read a line and store it in a string
		currentLine = "";
		getline(fileIn, currentLine, '\n');
		cout << "Reading line: " << currentLine << endl; 

		index = 0; //Move to the start of the line (index 0). 
		while (currentLine[index] != '\0') //read the line (character-by-character)
		{

			//The case when a tag's opening bracket is found 
			if (currentLine[index] == '<')
			{

				//In case the current tag is the xml prolog
				if (currentLine[index + 1] == '?' && currentLine[index + 2] == 'x' && currentLine[index + 3] == 'm' && currentLine[index + 4] == 'l') //If '<' is succeeded by "?xml" then check for prolog's end "?>". 
				{
					while (!(currentLine[index] == '?' && currentLine[index + 1] == '>'))
						index++;

					if (tags.isEmpty()) //If no tag has been found before the prolog then it means that the prolog is at the beginning of the document. 
						prolog = true;
					else errors.push(XMLData(line, "<?xml?>"));
					index += 2;

				}
				if (index >= currentLine.length()) continue;

				//In case the current tag is a comment
				if (currentLine[index + 1] == '!' && currentLine[index + 2] == '-' && currentLine[index + 3] == '-') //If '<' is succeeded by "!--" then check for comment's end "-->". 
				{
					while (!(currentLine[index] == '-' && currentLine[index + 1] == '-' && currentLine[index + 2] == '>'))
						index++;
					if (currentLine[index] == '-' && currentLine[index + 1] == '-' && currentLine[index + 2] == '>') //If the comment does eventually close
					{
						index += 3;
						continue;
					}
				}

				if (index >= currentLine.length()) continue;
				//In case the tag is a closing tag for some tag which might have been opened previously
				if (currentLine[index + 1] == '/') //If this tag is a closing tag
				{
					index += 2;
					string thisTag = "<";
					while (currentLine[index] != '>') thisTag += currentLine[index++];
					thisTag += '>';
					if (tags.getTop().tag == thisTag)
						tags.pop();
					else
						tags.push(XMLData(line, thisTag));
					index++;
				}
				if (index >= currentLine.length()) continue;

				//The case that the tag is just an opening tag
				else
				{
					XMLData newTag(line);
					string tagName = "<";
					if (currentLine[index] == '<') index++;

					while (currentLine[index] != '>') //until the tag ends
					{
						if (currentLine[index] == ' ' && currentLine[index + 1] != '>' && currentLine[index + 1] != ' ') //read attributes here
						{
							string att = "", val = "";

							while (currentLine[index] != '=') //Read the attribute name
							{
								if (currentLine[index] != ' ') //If current character is not a space, store it. (Ignores extra spaces)
								{
									att += currentLine[index];
								}
								index++;
							} //at '=' after this loop
							index++;//move one step ahead from the '=' sign. 
							while (currentLine[index] == ' ') index++; //ingoring extra spaces between '=' sign and the name of the attribute. 
							char quote = '_';
							while (currentLine[index] != ' ' && currentLine[index] != '>')
							{
								if ((currentLine[index] == '\'' || currentLine[index] == '\"') && quote == '_') quote = currentLine[index++]; //Store the quote to check balance; 
								if (quote != '_' && currentLine[index] == quote)
								{
									//If quote is balanced, i.e., an opening quote was found and now closing quote is also found. 
									quote = '+'; //Kind of marker to tell that quotes are balanced. 
									index++;
									break;
								}
								val += currentLine[index];
								index++;
							}
							//if (quote == '-') errors.push(XMLData(line, tagName)); //How to handle the case when there is not any quote or the quote is not matched? 
							if (quote == '+') newTag.newAttribute(att, val);

						}
						else
						{
							tagName += currentLine[index];
							index++;
						}
					}
					tagName += '>';
					newTag.setName(tagName);
					//the new tag was created here before. This creates the object after the readAttributes function is called. That function can not access this and store attributes in it. Call it before that function. 
					tags.push(newTag);
					index++;
				}
				if (index >= currentLine.length()) continue;

			}
			index++;
			
		}
	}
	fileIn.close();

	if (prolog && tags.isEmpty() && errors.isEmpty()) return true;
	while (!errors.isEmpty())
	{
		cout << errors.pop() << endl;
	}
	while (!tags.isEmpty())
	{
		cout << tags.pop() << endl;
	}
	return false;
}

int main()
{
	string filename1 = ""; //Enter the file name here or modify the main to take input from the user.
	readFile(filename1) ? cout << "File " << filename1 << " is ok. \n" : cout << "File " << filename1 << " is not ok. \n";
}
