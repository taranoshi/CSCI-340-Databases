﻿using System;
using DBOverflow.DBConnection;
using DBOverflow.DBOverflow.Interface;

namespace DBOverflow.DBOverflow
{
    public class DBOverflowApplication
    {
        //connecton details
        DBConnectionHandler dbconnectionHandle;
        private string serverName;
        private int port;
        private string userName;
        private string dbName;

        //user details
        private int currentUser;
        private string currentUserName;

        //application views
        private AskAQuestionUI askAQuestionUI;
        private QuestionBoardUI questionBoardUI;
        private QuestionViewUI questionViewUI;
        private UserSelectionUI userSelectionUI;

        public DBOverflowApplication(string serverName = "localhost", int port = 5432, string userName = "cslab", string dbName = "dboverflow")
        {
            this.serverName = serverName;
            this.port = port;
            this.userName = userName;
            this.dbName = dbName;
        }
        //this serves at the outside entry into this object, the rest of the functionality is handled internally.
        public void Run()
        {
            EstablishConnection();
            this.dbconnectionHandle.OpenConnectoin();
            SelectUser();
            ShowHomeMenu();
            this.dbconnectionHandle.CloseConnection();
        }
        //open a connection with the DB for making queries
        private void EstablishConnection()
        {
            this.dbconnectionHandle = new DBConnectionHandler(this.serverName, this.port, this.userName, this.dbName);
            this.dbconnectionHandle.Connect();
        }
        //select your username when first logging on, cannot be changed during the session (yet)
        private void SelectUser()
        {
            userSelectionUI = new UserSelectionUI(dbconnectionHandle);
            var userInfo = userSelectionUI.Show();
            this.currentUser = Convert.ToInt32(userInfo[0]);
            this.currentUserName = userInfo[1];
        }
        //handle the AskAQuestion UI page
        private void AskAQuestion()
        {
            this.askAQuestionUI = new AskAQuestionUI(this.currentUserName,this.currentUser, this.dbconnectionHandle);
            this.askAQuestionUI.Show();
            this.QuestionBoard("unanswered");
        }
        //show a list of questrions for the user to look at, depending on the type
        private void QuestionBoard(string type)
        {
            this.questionBoardUI = new QuestionBoardUI(this.currentUserName, this.dbconnectionHandle);
            while (true)
            {
                var qba = this.questionBoardUI.Show(type);
                if(qba[0] == "exit")
                    break;
                if (qba[0] == "view")
                    QuestionView(qba[1], qba[2]);
            }
        }
        //this is for displaying an individual question "up close"
        //this will also incorporate the ability to vote on questions
        private void QuestionView(string questionID)
        {
            questionViewUI = new QuestionViewUI(this.currentUserName, this.currentUser, this.dbconnectionHandle);
            questionViewUI.Show(questionID);
        }

        private void QuestionView(string questionID, string question)
        {
            questionViewUI = new QuestionViewUI(this.currentUserName, this.currentUser, this.dbconnectionHandle);
            questionViewUI.Show(questionID, question);
        }
        //this is for showing the home menu, where a user can select the general options to do
        private void ShowHomeMenu()
        {
            while(true)
            {
                Console.Clear();
                Console.WriteLine("Currently signed in as: {0}", this.currentUserName);
                Console.WriteLine(new String('-', Console.WindowWidth));
                Console.WriteLine("1) Ask a question.");
                Console.WriteLine("2) Answer a question.");
                Console.WriteLine("3) View Top Questions.");
                Console.WriteLine("4) View Random Questions.");
                Console.WriteLine();
                Console.WriteLine();
                Console.WriteLine("ESC) Quit the application.");
                Console.WriteLine(new String('-', Console.WindowWidth));
                Console.Write(">");
                var k = Console.ReadKey().Key;
                if (k == ConsoleKey.D1)
                {
                    AskAQuestion();
                }

                if (k == ConsoleKey.D2)
                {
                    QuestionBoard("unanswered");
                }

                if (k == ConsoleKey.D3)
                {
                    QuestionBoard("top");
                }

                if (k == ConsoleKey.D4)
                {
                    QuestionBoard("random");
                }

                if (k == ConsoleKey.Escape)
                {
                    break;
                }
            }
        }
    }
}
