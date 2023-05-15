<div align="center">

# LeagueGPT 

<img src="https://static.vecteezy.com/system/resources/previews/021/608/790/original/chatgpt-logo-chat-gpt-icon-on-black-background-free-vector.jpg" width="100">

A powerful AI chatbot for League of Legends

</div>

---

LeagueGPT uses OpenAI's GPT models to generate intelligent and contextually relevant responses to in-game chat messages. This project is for educational purposes only.


## Features

- **In-game chat analysis**: LeagueGPT can parse and respond to in-game chat messages in real-time, providing useful information and strategies to players.
- **Customizable responses**: The bot's responses can be customized based on the user's preferences.
- **Multi-threaded requests**: LeagueGPT can handle multiple chat requests simultaneously, ensuring a smooth and lag-free gaming experience.
- **Error handling**: The bot is designed to handle errors gracefully, providing useful feedback to the user when something goes wrong.

## Installation

1. Clone the repository to your local machine.
2. Install the required dependencies.
3. Go to your `Riot Games/League of Legends/Game` folder.
4. Copy all the files from the cloned repository's dependencies folder into the League of Legends folder.
5. Put the DLL file into the BGX Plugins folder (if you don't want to compile it yourself).
6. Start a game in League of Legends, inject BGX, load LeagueGPT, set your API key and necessary settings, and enjoy the game!

## Configuration

You can customize the behavior of LeagueGPT by modifying the configuration file and the `CommunityPrompts.json` file in the League of Legends folder. Here are some of the options you can configure:

- **Model**: The GPT model used by the bot. Options include "gpt-4", "gpt-4-0314", "gpt-3.5-turbo", and "gpt-3.5-turbo-0301".
- **Key**: Your OpenAI API key.
- **System**: The system message to initialize the chat with. This can be edited from the `CommunityPrompts.json` file. You can use `{champName}` as a variable, and the script will automatically detect and replace it.
- **User**: The user message to initialize the chat with. This is automatically detected by the script and ignores messages from your hero.

Example `CommunityPrompts.json`:

```json
{
    "description": ".You are playing {champName}. If a message is not clearly intended for you or not insinuated towards you, type 'Skip'",
    "icon": "",
    "ignore_key": "Skip",
    "title": "toxic gamer"
}
```
## Contributing

Contributions to LeagueGPT are welcome! Please see the [contribution guidelines](CONTRIBUTING.md) for more information.

## License

LeagueGPT is licensed under the [MIT License](LICENSE.md).

## Note

Please note that you need SDK access for loading this plugin. The release of this plugin is not possible as BGX does not allow third-party operations such as web requests or executing external programs. The included executable `LeagueGPTHelper.exe` is a basic C# program that handles making OpenAI requests. The source code can be viewed in the `LeagueGPTHelper` folder or by using a program like dnSpy, as the program is not encrypted.