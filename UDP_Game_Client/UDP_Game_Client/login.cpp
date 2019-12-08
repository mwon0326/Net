#include "login.h"

int con = 0;

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int TCP_Login()
{
	int retval;

	printf("���� ����\n\n");
	printf("����, ����, Ȥ�� �밢������ ���� 5���� ���� ���޾� ��ġ��Ű�� �̱�� ����\n");
	printf("����� 2���� �����ư��鼭 ���� ��ġ��ŵ�ϴ�\n");
	printf("�鵹�� ���� �����մϴ�\n");

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ���
	while (1) {
		// ������ �Է�
		printf("\n��й�ȣ�� �Է��ϼ��� : ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// ������ ������
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		int rBuf;

		// ������ �ޱ�
		retval = recvn(sock, (char*)&rBuf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		if (rBuf == 0)
		{
			printf("��ġ�մϴ�. ������ �����մϴ�.\n");
			break;
		}
		else if (rBuf == 1)
		{
			printf("����ġ\n��й�ȣ�� �ٽ� �Է����ּ���\n");
		}
		else
		{
			printf("�� ������ �źι��߽��ϴ�.\n");
			con = 1;
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return con;
}