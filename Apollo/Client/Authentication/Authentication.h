#pragma once
#include <random>
#include <regex>
#include <lmcons.h>

class Authentication {
public:
	inline static std::string getUsername()
	{
		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;

		GetUserNameA(username, &username_len);

		return std::string(username);
	}

	static inline bool Authorized = false;

	static inline __int32 key = 0;
	static inline bool held = false;

	static inline char usernameQuery[256];
	static inline char usernameH[2] = { 0 };
	
	static inline char passwordQuery[256];
	static inline char passwordH[2] = { 0 };

	static inline bool pressedEnter = false;

	static inline bool clickedUsername = false;
	static inline bool clickedPass = false;

	static void UpdateKeyCode(__int32 keyS, bool heldS) {
		if (heldS)
			return;

		key = keyS;
		held = heldS;

		if ((key == VK_BACK && !held) && clickedUsername) {
			int len = (int)strlen(usernameQuery);
			if (len > 0) {
				usernameQuery[len - 1] = '\0';
			}
		}
		else if ((key >= 65 && key <= 90 && !held) && clickedUsername) {
			usernameH[0] = (char)key;
			if (usernameH[0] != '\0') {
				strcat_s(usernameQuery, usernameH);
			}
		}

		if ((key == VK_BACK && !held) && clickedPass) {
			int len = (int)strlen(passwordQuery);
			if (len > 0) {
				passwordQuery[len - 1] = '\0';
			}
		}
		else if ((key >= 65 && key <= 90 && !held) && clickedPass) {
			passwordH[0] = (char)key;
			if (passwordH[0] != '\0') {
				strcat_s(passwordQuery, passwordH);
			}
		}

		if (key == Keys::ENTER && !held) {
			pressedEnter = true;
		}
	}

	static void RenderUI() {
		if ((Authorized && !Global::renderUI) || Address::getClientInstance() == nullptr)
			return;

		Player* player = Address::getLocalPlayer();
		ClientInstance* instance = Address::getClientInstance();
		GuiData* guidata = instance->getGuiData();

		instance->grabMouse();

		float RenderX = guidata->mcResolution.x / 2;
		float RenderY = (guidata->mcResolution.y / 2);

		Vector4<float> Rectangle(RenderX - 500, RenderY - 300, RenderX + 500, RenderY + 300);
		ImRenderUtil::fillRectangle(Rectangle, UIColor(15, 13, 15), 1, 20);

#pragma region ClientUI
		if (Authorized) {
			Vector4<float> UIPos (Rectangle.x + 40, Rectangle.y + 40, Rectangle.z - 40, Rectangle.w - 40);
			Vector4<float> UILeft (Rectangle.x + 40, Rectangle.y + 40, Rectangle.x + 150, Rectangle.w - 40);

			ImRenderUtil::fillRectangleCustom(UILeft, UIColor(29, 29, 29), 1, Vector4<float>(20, 20, 0, 0));
		}
#pragma endregion

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
#pragma region AuthLogin
		if (!Authorized) {
			Vector4<float> PlayerUI(RenderX - 90, RenderY - 200, RenderX + 90, RenderY - 10);
			Vector4<float> UsernameUI(PlayerUI.x - 20, PlayerUI.w + 20, PlayerUI.z + 20, PlayerUI.w + 55);
			Vector4<float> PassUI(UsernameUI.x, UsernameUI.w + 10, UsernameUI.z, UsernameUI.w + 45);
			Vector2<float> RememberMeUI(PassUI.x + 5.5, PassUI.w + 15);

			static bool RememberMe = false;

			UIColor rgb = ColorUtils::Rainbow(1.7, 1, 1, 1);

			UIColor UsernameOutline(20, 17, 20);
			UIColor PassOutline(20, 17, 20);

			std::string UsernameQuery = usernameQuery;
			std::string PasswordQuery = passwordQuery;

			std::transform(UsernameQuery.begin(), UsernameQuery.end(), UsernameQuery.begin(), [](unsigned char c) {
				return std::tolower(c);
				});

			std::transform(PasswordQuery.begin(), PasswordQuery.end(), PasswordQuery.begin(), [](unsigned char c) {
				return std::tolower(c);
				});

			std::string Username = strlen(usernameQuery) == 0 ? xorstr_("Username") : UsernameQuery;
			std::string Password = strlen(passwordQuery) == 0 ? xorstr_("Password") : PasswordQuery;
			std::string RememberMeText = xorstr_("Remember me");

			ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(PlayerUI.x, PlayerUI.y), ImVec2(PlayerUI.z, PlayerUI.w), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 30);

			if (clickedUsername) {
				UsernameOutline = UIColor(22, 51, 65);
				PassOutline = UIColor(20, 17, 20);
				ImRenderUtil::fillShadowRectangle(UsernameUI, UsernameOutline, 1, 20, 0, 3.5);
			}
			if (clickedPass) {
				PassOutline = UIColor(22, 51, 65);
				UsernameOutline = UIColor(20, 17, 20);
				ImRenderUtil::fillShadowRectangle(PassUI, PassOutline, 1, 20, 0, 3.5);
			}
			ImRenderUtil::fillRectangle(UsernameUI, UIColor(20, 17, 20), 1, 3.5);
			ImRenderUtil::drawRoundRect(UsernameUI, 0, 3.5, UsernameOutline, 1, 1.5);
			ImRenderUtil::fillRectangle(PassUI, UIColor(20, 17, 20), 1, 3.5);
			ImRenderUtil::drawRoundRect(PassUI, 0, 3.5, PassOutline, 1, 1.5);

			ImRenderUtil::fillCircle(RememberMeUI, 6, RememberMe ? rgb : UIColor(15, 15, 15), 1, 12.f);
			ImRenderUtil::fillShadowCircle(RememberMeUI, 6, RememberMe ? rgb : UIColor(15, 15, 15), 1, 40, 0);

			//ImRenderUtil::Blur(UsernameUI, 3.5, 7.5);

			ImRenderUtil::drawText(Vector2<float>(UsernameUI.x + 15, UsernameUI.y + 8), &Username, UIColor(115, 114, 113), 1, 1, false);
			ImRenderUtil::drawText(Vector2<float>(PassUI.x + 15, PassUI.y + 8), &Password, UIColor(115, 114, 113), 1, 1, false);
			ImRenderUtil::drawText(Vector2<float>(RememberMeUI.x + 15, RememberMeUI.y - 8.3), &RememberMeText, UIColor(115, 114, 113), 1, 1, false);

			if (ImRenderUtil::isMouseOver(UsernameUI)) {
				UsernameOutline = UIColor(22, 51, 65);
				PassOutline = UIColor(20, 17, 20);

				ImRenderUtil::drawRoundRect(UsernameUI, 0, 3.5, UsernameOutline, 1, 1.5);

				if (Utils::leftClick) {
					clickedUsername = true;
					clickedPass = false;
					Utils::leftClick = false;
				}
			}
			else if (ImRenderUtil::isMouseOver(PassUI)) {
				UsernameOutline = UIColor(20, 17, 20);
				PassOutline = UIColor(22, 51, 65);

				ImRenderUtil::drawRoundRect(PassUI, 0, 3.5, PassOutline, 1, 1.5);

				if (Utils::leftClick) {
					clickedUsername = false;
					clickedPass = true;
					Utils::leftClick = false;
				}
			}
			else if (ImRenderUtil::isMouseOver(Vector4<float>(PassUI.x, PassUI.w + 7, PassUI.z - 70, PassUI.w + 17))) {
				if (Utils::leftClick) {
					RememberMe = !RememberMe;
					UsernameOutline = UIColor(20, 17, 20);
					PassOutline = UIColor(20, 17, 20);
					clickedUsername = false;
					clickedPass = false;
					Utils::leftClick = false;
				}
			}
			else if (ImRenderUtil::isMouseOver(Rectangle)) {
				if (Utils::leftClick) {
					PassOutline = UIColor(20, 17, 20);
					UsernameOutline = UIColor(20, 17, 20);

					clickedUsername = false;
					clickedPass = false;
					Utils::leftClick = false;
				}
			}

			if (pressedEnter) {
				bool isTozic = Username == xorstr_("tozic") && Password == xorstr_("neverhax");
				bool isEpic = Username == xorstr_("epic") && Password == xorstr_("skidder");
				bool isDark = Username == xorstr_("dark") && Password == xorstr_("fatredstoneeater");

				std::string user = Username;

				std::string role = xorstr_("Previewer");

				if (isTozic || isEpic || isDark) {
					role = xorstr_("Dev");

					Global::Notification.addNotification(xorstr_("Authentication"), Utils::combine(xorstr_("Authenticated as "), Username, xorstr_(" ("), role, xorstr_(")")), 6);

					Global::Username = Username;

					Authorized = true;
				}

				if ((RememberMe) && isTozic) { // && (isTozic)
					if (!FileUtils::doesPathExist(FileUtils::getAuthPath())) {
						FileUtils::writeText(FileUtils::getAuthPath(), Utils::combine(Username.c_str(), "\n", Password.c_str()));
					}
					else {
						FileUtils::deletePath(FileUtils::getAuthPath());
						FileUtils::writeText(FileUtils::getAuthPath(), Utils::combine(Username.c_str(), "\n", Password.c_str()));
					}
				}
				pressedEnter = false;
			}
		}
#pragma endregion
		ImGui::PopFont();
	}

	static std::vector<std::string> readLinesFromTxt(const std::string& filename) {
		std::ifstream file(filename);
		std::vector<std::string> lines;

		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				// Trim leading and trailing whitespace
				line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

				// Check if the line is empty or starts with "//" after trimming whitespace
				if (line.empty()/* || line.find("//") == 0*/) {
					continue;
				}
				lines.push_back(line);
			}
			file.close();
		}
		else {
			Global::Notification.addNotification(xorstr_("Authentication"), xorstr_("failed authentication [Error: 405]"), 10);
		}

		return lines;
	}

	static void Authorize() {
		if (FileUtils::doesPathExist(FileUtils::getAuthPath())) {
			std::string filename = FileUtils::getAuthPath();
			std::vector<std::string> lines = readLinesFromTxt(filename);

			std::string Username = lines[0];
			std::string Password = lines[1];

			bool isTozic = Username == xorstr_("tozic") && Password == xorstr_("neverhax");
			bool isEpic = Username == xorstr_("epic") && Password == xorstr_("skidder");
			bool isDark = Username == xorstr_("dark") && Password == xorstr_("fatredstoneeater");

			std::string role = xorstr_("Previewer");

			if (isTozic || isEpic || isDark) {
				role = xorstr_("Dev");

				Global::Notification.addNotification(xorstr_("Authentication"), Utils::combine(xorstr_("Authenticated as "), Username, xorstr_(" ("), role, xorstr_(")")), 6);

				Global::Username = Username;

				Authorized = true;
			}
		}
	}
};