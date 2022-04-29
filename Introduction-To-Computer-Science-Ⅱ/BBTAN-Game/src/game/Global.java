/********************************
 *	P.Y. copyright	 			*
 *	Global Manager        		*
 *	Status: processing     		*
 ********************************/
package game;

public class Global {

    public static int level = 0;
    public static int difficulty = 1;
    public static int num = 1; // TODO: isn't implemented, use as the num as level'
    public final static double BLOCK_WIDTH = 50;
    public final static double BALL_WIDTH = 25;
    //public static int bottom = ;  // const
    private static int highestScore = 0; // read a file
    private static String highestScoreName = "";

    //public static int getLevel() {return level;}
    //public static void addLevel() {level++;}
    public static int getDifficulty() {return difficulty;}
    public static void setDifficulty(int dif) {difficulty = dif;}
    //public static int getBottom() {return bottom;}
    public static void storeHSName(String name) {highestScoreName = name;}
    public static String getHSName() {return highestScoreName;}
    public static void storeHS(int score) {highestScore = score;}
    public static int getHS() {return highestScore;}
}