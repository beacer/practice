package inheritance;

/**
 * Created by beacer on 16-7-2.
 */
public class Manager extends Employee {
    private double bonus;

    public Manager(String name, double salary, int year, int month, int day) {
        super(name, salary, year, month, day);
        this.bonus = 0;
    }

    public double getSalary() {
        return super.getSalary() + this.bonus;
    }

    public void setBonus(double bonus) {
        this.bonus = bonus;
    }
}
