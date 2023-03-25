void display(int a, int b, int c) //Quadratic equation of the form ax²+bx+c
{
	char pow = char(253); //ASCII for ²
	if (a != 0) {
		if (a != 1)
			if (a != -1)
				cout << a;
			else
				cout << "-";
		cout << "x" << pow << " ";
	}
	if (b != 0) {
		if (a != 0)
			(b > 0) ? cout << "+ " << b : cout << "- " << b * -1;
		else
			cout << b;
		if (b == 1 || b == -1) cout << "\b";
		cout << "x ";
	}
	if (c != 0)
	{
		if (b != 0)
			(c > 0) ? cout << "+ " << c : cout << "- " << c * -1;
		else
			cout << c;
	}
	cout << "\n";
}