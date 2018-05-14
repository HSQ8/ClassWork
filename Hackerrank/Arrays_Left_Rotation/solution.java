import java.io.*;
import java.util.*;
import java.text.*;
import java.math.*;
import java.util.regex.*;

public class Solution {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int n = in.nextInt();
        int k = in.nextInt();
        int a[] = new int[n];
        int solution[] = new int[n];
        for(int a_i=0; a_i < n; a_i++){
            a[a_i] = in.nextInt();
        }
        
        for(int i = 0; i<n-k; i++)
            solution[i] = a[i+k];
        for(int j = 0; j<k; j++)
            solution[n-k+j] = a[j];
        
        
        printArray(solution);
    }
    public static void printArray(int[] _a){
            for(int i = 0; i< _a.length-1; i++)
                System.out.print(_a[i]+" ");
            System.out.print(_a[_a.length-1]);
            
        }
}
