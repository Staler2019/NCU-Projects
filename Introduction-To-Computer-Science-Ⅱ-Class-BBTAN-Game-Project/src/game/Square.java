/********************************
 *	P.Y. copyright	 			*
 *	Class inherits Block	   	*
 *	Status: finished	      	*
 ********************************/
package game;

import javafx.scene.control.Button;

public class Square extends Block {

    public Square(Button btn) {
        super(btn);
    }

    @Override
    public void minusNum(Ball b) { // "this" is block // include ball rebounding
      double x0 = b.getPosi_x() + Global.BALL_WIDTH / 2; // ball
        double y0 = b.getPosi_y() + Global.BALL_WIDTH / 2;
        double x1 = this.getPosi_x() + Global.BLOCK_WIDTH / 2; // block
        double y1 = this.getPosi_y() + Global.BLOCK_WIDTH / 2;
        if (this.pythagorean(Math.abs(x0 - x1) - Global.BLOCK_WIDTH / 2,
                Math.abs(y0 - y1) - Global.BLOCK_WIDTH / 2) <= Global.BALL_WIDTH / 2
                && Math.abs(x0 - x1) > (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2
                && Math.abs(y0 - y1) > (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2) {
            this.num--;
            b.revertAtVertex(this); // this is a condition has friction
        } else if (Math.abs(x0 - x1) <= (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2 + Block.ERROR
                && Math.abs(y0 - y1) < (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2) {
            this.num--;
            b.revertY();
        } else if (Math.abs(y0 - y1) <= (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2 + Block.ERROR
                && Math.abs(x0 - x1) < (Global.BALL_WIDTH + Global.BLOCK_WIDTH) / 2) {
            this.num--;
            b.revertX();
        }
    }
}