package t2;
/**  Cave Game  Program Code
     Copyright (c) 1999 James M. Bieman

     To compile: javac CaveGame.java
     To run:     java CaveGame

     The main routine is CaveGame.main
				    
**/

// class Wall

import java.io.*;

public class Wall implements CaveSite {

 public void enter(Player p)
	     {
   System.out.println("Ouch! That hurts.");
 }

}

