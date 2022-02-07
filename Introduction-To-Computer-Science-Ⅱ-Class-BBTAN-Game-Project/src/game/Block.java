/********************************
 *	P.Y. copyright	 			*
 *	Abstract Class Block        *
 *	Status: finished            *
 ********************************/
package game;

import javafx.scene.control.Button;

public abstract class Block {   // blocks: rectangle|| doing, triangle|| plaining, 1/4circle|| plaining

    protected int num;
    protected Button button; // size 50x50 pixel // TODO: color implement in button's style'
    protected final static double ERROR = 0.1;
    //private String color[] = new String[10]{("Green"), ("")};

    public Block(Button btn) {
        this.button = btn;
        this.num = (int)(Math.random()*2) + Global.level - 1;
        if(this.num <= 0) {
            this.num = 1;
        }
        //this.checkColor();
    }

    public int getNum() {return this.num;}

    // get before move block to next level
    public double getPosi_x() {return this.button.getLayoutX();}

    public double getPosi_y() {return this.button.getLayoutY();}

    // move downward directly without using setPosi
    public void nextLevel() {this.button.setLayoutY(this.button.getLayoutY()-50);}

    // done rectangle hit by ball   // include ball rebounding
    public abstract void minusNum(Ball b);

    protected double pythagorean(double a, double b) {
        return Math.sqrt(Math.pow(a, 2) + Math.pow(b, 2));
    }
/*
    protected void checkColor() {
        String[10] = {"Green", };
        if(this.num >= 10){
            this.button.setStyle(); // TODO: color
        }
        else if
    }
*/
    public void movePosi_x(double x){
        this.button.setLayoutX(this.button.getLayoutX() + x);
    }

    public void movePosi_y(double y){
        this.button.setLayoutY(this.button.getLayoutY() + y);
    }

    public void movePosi(double x, double y){
        this.movePosi_x(x);
        this.movePosi_y(y);
    }

    public Button getBTN() {
        return this.button;
    }
}