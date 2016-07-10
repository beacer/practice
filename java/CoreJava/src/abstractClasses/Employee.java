package abstractClasses;
import java.util.Date;
import java.util.GregorianCalendar;

/**
 *
 */
public class Employee extends Person {
    private double salary;
    private Date hireDay;

    public Employee(String name, double salary, int year, int month, int day) {
        super(name);
        this.salary = salary;
        GregorianCalendar calendar = new GregorianCalendar(year, month - 1, day);
        this.hireDay = calendar.getTime();
    }

    public double getSalary() {
        return this.salary;
    }

    public Date getHireDay() {
        return this.hireDay;
    }

    public String getDescription() {
        return String.format("an employee with salary of %.2f", this.salary);
    }

    public void raiseSalary(double byPercent) {
        this.salary += this.salary * byPercent / 100;
    }
}
