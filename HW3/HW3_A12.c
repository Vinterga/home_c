#include <stdio.h>
#include <stdlib.h>

int main()
{
    char a, b, c; //������ ���������� ����������, ��� �����, �.�. � ������� ������ �� ���� �������� ������������� ����������� �����, �.�. �� ������ ����� �� ������

    scanf("%c%c%c", &a, &b, &c); //������ ���������� ���������� � ������� ������, ��� �������� � ���������, ����� ��� ������� ������ ��������� ��� ����� ����������� �����

    int n1 = a - '0'; // ���������� ������� ���������� ����� ������: ������ ������������� ���������� � �������� �� ��� "0" ��� �������������� ������� � ���� ����� � ��������������� ����� �����
    int n2 = b - '0'; // ���������� ������� ���������� ����� ������: ������ ������������� ���������� � �������� �� ��� "0" ��� �������������� ������� � ���� ����� � ��������������� ����� �����
    int n3 = c - '0'; // ���������� ������� ���������� ����� ������: ������ ������������� ���������� � �������� �� ��� "0" ��� �������������� ������� � ���� ����� � ��������������� ����� �����

    /*���� ������ ������������ ����� ����� �� 0 �� 9, �� ����� ������������� ���������� ���� ������ char � ������������� ���� ������ int,
    ������� �� ���� '0'. �.�. �������� ASCII ��� ���� �� 0 �� 9 ��������� � ��������� �� 48 �� 57, ��� ��������� ���� ����������� �������� ��������.
    ������ ������, ��� ��������� '0' �� ASCII-���� ������� ���������� �����, ������� ������������ ���� ������.
    ��� ���������� ������, ��� ������ '0' � ������� ASCII ����������� ����� ������ 47, � ������, ��������������, ���� ������� � '1' �� '9' ��� ������� 48-57.
    ����� �������, ��������� ����� ������� '0' �� ����� �������� '1' - '9' ���� �� �������� ��������� ��������*/

    int sum = n1 + n2 + n3;

    printf("%d", sum);

    return 0;
}
